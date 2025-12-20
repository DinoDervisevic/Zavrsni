#ifndef FOREVER_HPP
#define FOREVER_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Forever : public Block {
    BlockSequence* block_sequence;
public:
    Forever(BlockSequence* block_sequence) : Block("Control", "Forever"), block_sequence(block_sequence) {}

    double execute(Robot& robot) override {
        if (block_sequence == nullptr) {
            return 0;
        }
        
        block_sequence->execute(robot);
        double t = block_sequence->get_time_left();
        block_sequence->set_time_left(0);

        if (block_sequence->get_current_block() == nullptr) block_sequence->reset(robot);

        return t;
    }

    bool done(Robot& robot) override {
        return false;
    }
};

#endif