#ifndef LESS_THAN_HPP
#define LESS_THAN_HPP

#include "../common_block_includes.hpp"

using namespace std;

class LessThan : public Block {
    Block* value1;
    Block* value2;
public:
    LessThan(Block* value1, Block* value2) : Block("Operator", "LessThan"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) < value2->execute(robot);
    }

    string executeString(Robot& robot) override {
        if(value1->execute(robot) < value2->execute(robot)){
            return "True";
        } else {
            return "False";
        }
    }
};

#endif