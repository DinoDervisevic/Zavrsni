#ifndef IF_HPP
#define IF_HPP

#include "../common_block_includes.hpp"

using namespace std;

class If : public Block {
    BlockSequence* block_sequence;
    bool condition_checked, when_done = false;
    Block* condition;
public:
    If(BlockSequence* block_sequence, Block* condition) : Block("Control", "If"), block_sequence(block_sequence), condition(condition) {}

    double execute(Robot& robot) override {
        if(condition_checked || condition->execute(robot)){
            condition_checked = true;
            block_sequence->execute(robot);
            double t = block_sequence->get_time_left();
            block_sequence->set_time_left(0);
            if (block_sequence->get_current_block() == nullptr) {
                when_done = true;
                block_sequence->reset(robot);
            }
            return t;
        }
        else{
            when_done = true;
        }
        return 0;
    }

    void finish(Robot& robot) override {
        condition_checked = false;
        when_done = false;
    }

    bool done(Robot& robot) override {
        return when_done;
    }
};

#endif