#ifndef WHEN_CONDITION_HPP
#define WHEN_CONDITION_HPP

#include "../common_block_includes.hpp"

using namespace std;

class WhenCondition : public Block {
    Block* condition;
public:
    WhenCondition(Block* condition) : Block("Event", "WhenCondition"), condition(condition) {}

    double execute(Robot& robot) override {
        return 0;
    }

    bool done(Robot& robot) override {
        if (condition->execute(robot)) {
            return true;
        }
        return false;
    }
};


#endif