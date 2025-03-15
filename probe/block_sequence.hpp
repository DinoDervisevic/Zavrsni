#ifndef BLOCKSEQUENCE_HPP
#define BLOCKSEQUENCE_HPP

#include <vector>
#include <memory>
#include "blocks.hpp"
#include "robot.hpp"


class BlockSequence {
    Block* current_block;
    double time_left;
    bool is_running = false;
public:
    BlockSequence(Block* first_block) : current_block(first_block), time_left(0) {}

    void execute(Robot& robot) {
        if(time_left != 0){
            time_left -= robot.discrete_time_interval;
            time_left = max(time_left, 0.0);
        }
        while(time_left == 0 || current_block != nullptr) {
            time_left += current_block->execute(robot);
            current_block = current_block->next;
        }
    }

    double get_time_left() {
        return time_left;
    }

    Block* get_start_block() const {
        return current_block;
    }

    void set_is_running(bool is_running) {
        this->is_running = is_running;
    }

    bool get_is_running() {
        return is_running;
    }
};

#endif // BLOCKSEQUENCE_HPP