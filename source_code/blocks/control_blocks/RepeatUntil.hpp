#ifndef REPEAT_UNTIL_HPP
#define REPEAT_UNTIL_HPP

#include "../common_block_includes.hpp"

using namespace std;

class RepeatUntil : public Block {
    Block* condition;
    BlockSequence* block_sequence;
    bool when_done = false;
public:
    RepeatUntil(Block* condition, BlockSequence* block_sequence) : Block("Control", "RepeatUntil"), condition(condition), block_sequence(block_sequence) {}

    double execute(Robot& robot) override {
        if (condition == nullptr) {
            when_done = true;
            return 0;
        }
        if (block_sequence == nullptr) {
            return 0;
        }

        if (block_sequence->get_current_block() == nullptr) {
            block_sequence->reset(robot);
        }

        if(block_sequence->get_current_block() == block_sequence->get_starting_block() && condition->execute(robot)){
            when_done = true;
            block_sequence->reset(robot);
            return 0;
        }

        block_sequence->execute(robot);
        double t = block_sequence->get_time_left();
        block_sequence->set_time_left(0);

        return t;
    }

    void finish(Robot& robot) override {
        when_done = false;
    }

    bool done(Robot& robot) override {
        return when_done;
    }
};

#endif