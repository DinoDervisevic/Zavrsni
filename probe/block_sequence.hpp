#ifndef BLOCKSEQUENCE_HPP
#define BLOCKSEQUENCE_HPP

#include <vector>
#include <memory>
#include "blocks.hpp"
#include "robot.hpp"


class BlockSequence {
    Block* current_block;
    double time_left;
public:
    BlockSequence(Block* first_block) : current_block(first_block), time_left(0) {}

    void execute(Robot& robot) {
        while(time_left == 0 || current_block != nullptr) {
            time_left += current_block->execute(robot);
            current_block = current_block->next;
        }
    }

    double get_time_left() {
        return time_left;
    }
};

#endif // BLOCKSEQUENCE_HPP