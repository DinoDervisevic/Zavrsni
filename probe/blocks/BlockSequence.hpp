#ifndef BLOCKSEQUENCE_HPP
#define BLOCKSEQUENCE_HPP

#include <string>
#include <iostream>

using namespace std;

#include "../robot/Robot.hpp"
#include "Block.hpp"

class BlockSequence {
    Block* current_block;
    Block* starting_block;
    double time_left = 0;
    bool can_execute = true;
public:
    BlockSequence(Block* first_block) : current_block(first_block), starting_block(first_block),  time_left(0) {}

    void execute(Robot& robot) {
        
        while(true) {
            if(time_left != 0.0){
                time_left -= robot.discrete_time_interval;
                time_left = max(time_left, 0.0);
                break;
            }

            if(current_block == nullptr) {
                time_left += robot.discrete_time_interval;
                break;   
            }

            if(can_execute){
                time_left += current_block->execute(robot);
                can_execute = false;
                continue;
            }

            if(current_block->done(robot)){
                current_block->finish(robot);
                current_block = current_block->next;
                can_execute = true;
                continue;
            }
            else can_execute = true;

            break;
        }
    }

    void check_interferences(Robot& robot) {
        for(auto& sequence : robot.block_sequences){
            if(sequence == this) continue;
            this->current_block->deal_with_interference(robot, sequence);
        }
    }

    void reset(Robot& robot) {
        if(current_block != nullptr) current_block->finish(robot);
        current_block = starting_block;
        time_left = 0;
    }

    void stop(Robot& robot) {
        current_block = starting_block;
        time_left = 0;
    }

    double get_time_left() {
        return time_left;
    }

    void set_time_left(int amount){
        time_left = amount;
    }

    Block* get_current_block() const {
        return current_block;
    }

    Block* get_starting_block() const {
        return starting_block;
    }
};


#endif