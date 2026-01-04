#include "../include/AIUtil/AISessionIdGenerator.h"

AISessionIdGenerator::AISessionIdGenerator()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

std::string AISessionIdGenerator::generate()
{
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    long long randVal = std::rand() % 100000;
    long long rawId = now ^ randVal;
    return std::to_string(rawId);
}

