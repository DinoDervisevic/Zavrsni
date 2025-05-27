#ifndef STOP_OTHER_STACKS_HPP
#define STOP_OTHER_STACKS_HPP

#include "../common_block_includes.hpp"

using namespace std;

class StopOtherStacks : public Block {
public:
    StopOtherStacks() : Block("Control", "StopOtherStacks") {}

    double execute(Robot& robot) override {
        for (auto& stack : robot.block_sequences) {
            if(stack->get_current_block() != nullptr && stack->get_current_block() != this) stack->reset(robot);
        }

        robot.reset();
        return 0;
    }
};

#endif