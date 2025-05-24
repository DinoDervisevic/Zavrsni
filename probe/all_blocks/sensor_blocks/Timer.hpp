#ifndef TIMER_HPP
#define TIMER_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Timer : public Block {
public:
    Timer() : Block("Sensor", "Timer") {}

    double execute(Robot& robot) override {
        return robot.time_since_start;
    }
}; 

#endif