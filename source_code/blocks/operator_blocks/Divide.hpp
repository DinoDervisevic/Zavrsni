#ifndef DIVIDE_HPP
#define DIVIDE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Divide : public Block {
    Block* value1;
    Block* value2;
public:
    Divide(Block* value1, Block* value2) : Block("Operator", "Divide"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) / value2->execute(robot);
    }
};

#endif