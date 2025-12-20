#ifndef CONTROL_STOP_HPP
#define CONTROL_STOP_HPP

#include "../common_block_includes.hpp"

using namespace std;

class ControlStop : public Block {
    string option;
public:
    ControlStop(string option) : Block("Control", "ControlStop"), option(option) {}

    double execute(Robot& robot) override {
        if(option == "all"){
            for (auto& stack : robot.block_sequences) {
                if(stack->get_current_block() != nullptr && stack->get_current_block() != this) stack->reset(robot);
            }
            
            for (auto& stack : robot.block_sequences) {
                if(stack->get_current_block() != nullptr && stack->get_current_block() == this) stack->reset(robot);
            }

            robot.reset();
        }
        else if(option == "this stack"){
            for (auto& stack : robot.block_sequences) {
                if(stack->get_current_block() != nullptr && stack->get_current_block() == this) stack->reset(robot);
            }
        }
        else if(option == "and exit program"){
            return -404;
        }
        return 0;
    }
};

#endif