#ifndef RESET_TIMER_HPP
#define RESET_TIMER_HPP

#include "../common_block_includes.hpp"

using namespace std;

class ResetTimer : public Block {
public:
    ResetTimer() : Block("Sensor", "ResetTimer") {}

    double execute(Robot& robot) override {
        robot.time_since_start = 0;
        return 0;
    }
};

#endif
