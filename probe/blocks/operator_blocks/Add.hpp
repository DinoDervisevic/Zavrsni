#ifndef ADD_HPP
#define ADD_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Add : public Block {
    Block* value1;
    Block* value2;
public:
    Add(Block* value1, Block* value2) : Block("Operator", "Add"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) + value2->execute(robot);
    }
};

#endif