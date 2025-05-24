#ifndef IF_ELSE_HPP
#define IF_ELSE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class IfElse : public Block {
    BlockSequence* block_sequence1;
    BlockSequence* block_sequence2;
    bool condition_checked, else_checked, when_done = false;
    Block* condition;
public:
    IfElse(BlockSequence* block_sequence1, BlockSequence* block_sequence2, Block* condition) : Block("Control", "IfElse"), block_sequence1(block_sequence1), block_sequence2(block_sequence2), condition(condition) {}

    double execute(Robot& robot) override {
        double t = 0;
        if(condition_checked || (!condition_checked && !else_checked && condition->execute(robot))){
            condition_checked = true;
            block_sequence1->execute(robot);
            t = block_sequence1->get_time_left();
            block_sequence1->set_time_left(0);
        } else {
            else_checked = true;
            block_sequence2->execute(robot);
            t = block_sequence2->get_time_left();
            block_sequence2->set_time_left(0);
        }
        if((block_sequence1->get_current_block() == nullptr && condition_checked) || (block_sequence2->get_current_block() == nullptr && else_checked)){
            when_done = true;
            block_sequence1->reset(robot);
            block_sequence2->reset(robot);
        }
        return t;
    }

    void finish(Robot& robot) override {
        condition_checked = false;
        else_checked = false;
        when_done = false;
    }

    bool done(Robot& robot) override {
        return when_done;
    }
};

#endif