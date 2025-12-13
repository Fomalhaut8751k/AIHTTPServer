#include "../include/AiGame.h"

#include <chrono>
#include <thread>


AiGame::AiGame(int userId):
    gameOver_(false),
    userId_(userId),
    moveCount_(0),
    lastMove_(-1, -1),
    board_(BOARD_SIZE, std::vector<std::string>(BOARD_SIZE, EMPTY))
{
    srand(time(0));  // 初始化随机种子
}

bool AiGame::isDraw() const
{
    std::unique_lock<std::mutex> lock(mutex_);
    return moveCount_ >= BOARD_SIZE * BOARD_SIZE;
}

bool AiGame::humanMove(int x, int y)
{
    if(isValidMove(x, y))
    {
        std::unique_lock<std::mutex> lock(mutex_);
        moveCount_++;
        board_[x][y] = HUMAN_PLAYER;
        lastMove_ = {x, y};
        
        if(checkWin(x, y, HUMAN_PLAYER))
        {
            gameOver_ = true;
            winner_ = "human";
        }
        return true;
    }
    return false;
}

// 检查胜利条件
bool AiGame::checkWin(int x, int y, const std::string& player)
{   
    // 检查方向数组：水平，垂直，对角线，反对角线
    const int dx[] = {1, 0, 1, 1};
    const int dy[] = {0, 1, 1, -1};

    for(int dir = 0; dir < 4; dir++)
    {
        int count = 1;  // 当前位置已有一个棋子

        /*
            记 AI_PLAYER 为 A

            A   往左上角遍历，直到不是A为止
              A  
                [A]  当前位置，当前的位置要算在count上
                    A  往右下角遍历，直到不是A为止
        
            累计4个，小于5个，不满足胜利条件
        */

        // 正向检查
        for(int i = 1; i < 5; i++)
        {
            int newX = x + dx[dir] * i;
            int newY = y + dy[dir] * i;
            if(!isInBoard(newX, newY) || board_[newX][newY] != player){ break; }
            count++;
        }
        // 反向检查
        for(int i = 1; i < 5; i++)
        {
            int newX = x - dx[dir] * i;
            int newY = y - dy[dir] * i;
            if(!isInBoard(newX, newY) || board_[newX][newY] != player) { break; }
            count++;
        }

        if(count >= 5) { return true; }
    }
    return false;
}

void AiGame::aiMove()
{
    if(gameOver_ || isDraw()) { return; }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));  // 添加500ms延时
    int x, y;
    std::tie(x, y) = getBestMove();
    moveCount_++;
    board_[x][y] = AI_PLAYER;
    lastMove_ = {x, y};

    if(checkWin(x, y, HUMAN_PLAYER))
    {
        gameOver_ = true;
        winner_ = "ai";
    }
}

// 获取最后一步移动的坐标
std::pair<int, int> AiGame::getLastMove() const
{
    std::unique_lock<std::mutex> lock(mutex_);
    return lastMove_;
}

// 获取当前棋盘的状态
const std::vector<std::vector<std::string>>& AiGame::getBoard() const
{
    std::unique_lock<std::mutex> lock(mutex_);
    return board_;
}

bool AiGame::isGameOver() const
{
    std::unique_lock<std::mutex> lock(mutex_);
    return gameOver_;
}

std::string AiGame::getWinner() const
{
    std::unique_lock<std::mutex> lock(mutex_);
    return winner_;
}

// 检查移动是否有效
bool AiGame::isValidMove(int x, int y) const
{
    if(x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) { return false; }
    if(board_[x][y] != EMPTY) { return false; }
    if(gameOver_ || isDraw()) { return false; }
    return true;
}

// 检查是否坐标落在棋盘上
bool AiGame::isInBoard(int x, int y) const
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

// 获取AI的最佳移动位置
std::pair<int, int> AiGame::getBestMove()
{
    std::pair<int, int> bestMove = {-1, -1};  // 最佳落子位置
    int maxThreat = -1;   // 记录最大威胁分数

    // 1. 优先尝试进攻获胜或者阻止玩家胜利
    for(int r = 0; r < BOARD_SIZE; r++)
    {
        for(int c = 0; c < BOARD_SIZE; c++)
        {
            if(board_[r][c] != EMPTY) { continue; }  // 确保当前位置为空闲

            // 模拟AI落子， 判断是否可以获胜
            board_[r][c] = AI_PLAYER;
            if(checkWin(r, c, AI_PLAYER))
            {
                return {r, c};  // 立即获胜
            }
            board_[r][c] = EMPTY;  // 先尝试一下，如果不行再悔棋

            // 模拟玩家落子，判断是否需要防守
            board_[r][c] = HUMAN_PLAYER;
            if(checkWin(r, c, HUMAN_PLAYER))  // 如果玩家下了这位置就赢了，就必须阻止
            {
                board_[r][c] = AI_PLAYER;  // 恢复棋盘
                return {r, c};
            }
            board_[r][c] = EMPTY;
        }
    }

    // 2. 评估每个空格的威胁程度，选择最佳防守位置
    for(int r = 0; r < BOARD_SIZE; r++)
    {
        for(int c = 0; c < BOARD_SIZE; c++)
        {
            if(board_[r][c] != EMPTY) { continue; }  // 确保当前位置是空闲的

            int threatLevel = evaluateThreat(r, c);  // 评估威胁程度
            if(threatLevel > maxThreat)
            {
                maxThreat = threatLevel;
                bestMove = {r, c};
            }
        }
    }

    // 3. 如果找不到威胁点，选择靠近玩家或者已有棋子的空位
    if(bestMove.first == -1)
    {
        std::vector<std::pair<int, int>> nearCells;

        for(int r = 0; r < BOARD_SIZE; r++)
        {
            for(int c = 0; c < BOARD_SIZE; c++)
            {
                if(board_[r][c] != EMPTY && isNearOccupied(r, c))
                {   // 确保当前位置为空闲且靠近已有棋子
                    nearCells.push_back({r, c});
                } 
                
            }
        }

        // 如果找到靠近已有棋子的空位，随机选择一个
        if(!nearCells.empty())
        {
            int num = rand();
            board_[nearCells[num % nearCells.size()].first][nearCells[num % nearCells.size()].second] = AI_PLAYER;
            return nearCells[num % nearCells.size()]; 
        }
    }

    // 4. 如果所有策略都无效，选择第一个空位(保证AI落子)
    for(int r = 0; r < BOARD_SIZE; r++)
    {
        for(int c = 0; c < BOARD_SIZE; c++)
        {
            if(board_[r][c] == EMPTY)
            {
                board_[r][c] = AI_PLAYER;
                return {r, c};  // 返回第一个空位
            }
        }
    }

    board_[bestMove.first][bestMove.second] = AI_PLAYER;
    return bestMove;  // 返回最佳防守点或者其他策略的结果
}

// 评估威胁
int AiGame::evaluateThreat(int r, int c)
{   // 应该时把玩家最新的落子位置传入
    int threat = 0;

    // 检查四个方向上的玩家连子数
    const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    for(auto& dir: directions)
    {
        int count = 1;
        for(int i = 1; i <= 2; i++)
        {   // 探查2步
            int nr = r + i * dir[0], nc = c + i * dir[1];
            if(nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE && board_[nr][nc] == HUMAN_PLAYER)
            {
                count++;
            }
        }
        threat += count;  // 威胁分数累加
    }
    return threat;
}

// 判断某个空位是否靠近已有棋子
bool AiGame::isNearOccupied(int r, int c)
{
    // 当前位置(r, c)的八个方向
    const int directions[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
    };
    for(auto& dir: directions)
    {
        int nr = r + dir[0], nc = c + dir[1];
        if(nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE && board_[nr][nc] != EMPTY)
        {
            return true; // 该空位靠近已有棋子
        }
    }
    return false;
}