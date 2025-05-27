#ifndef SUBTRACT_HPP
#define SUBTRACT_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Subtract : public Block {
    Block* value1;
    Block* value2;
public:
    Subtract(Block* value1, Block* value2) : Block("Operator", "Subtract"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) - value2->execute(robot);
    }
};

#endif