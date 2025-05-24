#ifndef WHEN_TIMER_HPP
#define WHEN_TIMER_HPP

#include "../common_block_includes.hpp"

using namespace std;

class WhenTimer : public Block {
    Block* value;
public:
    WhenTimer(Block* value) : Block("Event", "WhenTimer"), value(value) {}

    double execute(Robot& robot) override {
        return 0;
    }

    bool done(Robot& robot) override {
        if(!is_number(value->executeString(robot)) || value->execute(robot) < 0){
            return false;
        }
        if(robot.time_since_start > value->execute(robot)){
            return true;
        } else {
            return false;
        }
    }
};

#endif