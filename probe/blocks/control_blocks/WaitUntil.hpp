#ifndef WAIT_UNTIL_HPP
#define WAIT_UNTIL_HPP

#include "../common_block_includes.hpp"

using namespace std;

class WaitUntil : public Block {
    Block* condition;
    bool when_done = false;
public:
    WaitUntil(Block* condition) : Block("Control", "WaitUntil"), condition(condition) {}

    double execute(Robot& robot) override {
        if(condition->execute(robot)){
            when_done = true;
            return 0;
        }
        return 0;
    }

    void finish(Robot& robot) override {
        when_done = false;
    }

    bool done(Robot& robot) override {
        return when_done;
    }
};

#endif