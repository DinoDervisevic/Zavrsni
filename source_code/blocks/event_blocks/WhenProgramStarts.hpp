#ifndef WHEN_PROGRAM_STARTS_HPP
#define WHEN_PROGRAM_STARTS_HPP

#include "../common_block_includes.hpp"

using namespace std;

class WhenProgramStarts : public Block {
public:
    WhenProgramStarts() : Block("Event", "WhenProgramStarts") {}

    double execute(Robot& robot) override {
        return 0;
    }

    bool done(Robot& robot) override {
        if(robot.time_since_start == 0){
            return true;
        }
        return false;
    }
};

#endif