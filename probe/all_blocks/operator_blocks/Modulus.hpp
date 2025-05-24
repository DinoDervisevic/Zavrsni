#ifndef MODULUS_HPP
#define MODULUS_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Modulus : public Block {
    Block* value1;
    Block* value2;
public:
    Modulus(Block* value1, Block* value2) : Block("Operator", "Modulus"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return static_cast<int>(value1->execute(robot)) % static_cast<int>(value2->execute(robot));
    }
};

#endif