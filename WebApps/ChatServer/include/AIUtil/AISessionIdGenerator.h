#ifndef AISESSIONIDGENERATOR_H
#define AISESSIONIDGENERTAOR_H

#include <chrono>
#include <random>
#include <cstdlib>
#include <ctime>
#include <string>

class AISessionIdGenerator
{
public:
    AISessionIdGenerator();
    std::string generate();
};

#endif