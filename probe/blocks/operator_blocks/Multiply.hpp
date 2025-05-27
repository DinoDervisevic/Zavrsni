#ifndef MULTIPLY_HPP
#define MULTIPLY_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Multiply : public Block {
    Block* value1;
    Block* value2;
public:
    Multiply(Block* value1, Block* value2) : Block("Operator", "Multiply"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) * value2->execute(robot);
    }
};

#endif