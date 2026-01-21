#ifndef CONTROL_STOP_HPP
#define CONTROL_STOP_HPP

#include "../common_block_includes.hpp"

using namespace std;

class ExitProgramException : public std::exception {
    public:
    const char* what() const noexcept override {
        return "Program exit requested";
    }
};

class ControlStop : public Block {
    string option;
public:
    ControlStop(string option) : Block("Control", "ControlStop"), option(option) {}

    double execute(Robot& robot) override {
        if(option == "all"){
            for (auto& stack : robot.block_sequences) {
                if(stack->get_current_block() != nullptr && stack->get_current_block() != this){ 
                stack->reset(robot);
                stack->set_time_left(robot.discrete_time_interval);
            }
            }
            
            for (auto& stack : robot.block_sequences) {
                if(stack->get_current_block() != nullptr && stack->get_current_block() == this){ 
                stack->reset(robot);
                stack->set_time_left(robot.discrete_time_interval);
            }
            }

            robot.reset();
        }
        else if(option == "this stack"){
            for (auto& stack : robot.block_sequences) {
                if(stack->get_current_block() != nullptr && stack->get_current_block() == this){ 
                stack->reset(robot);
                stack->set_time_left(robot.discrete_time_interval);
            }
            }
        }
        else if(option == "and exit program"){
            throw ExitProgramException();
        }
        return 0;
    }
};

#endif