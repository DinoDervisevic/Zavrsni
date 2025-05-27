#ifndef EQUALS_HPP
#define EQUALS_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Equals : public Block {
    Block* value1;
    Block* value2;
public:
    Equals(Block* value1, Block* value2) : Block("Operator", "Equals"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) == value2->execute(robot);
    }

    string executeString(Robot& robot) override {
        if(value1->execute(robot) == value2->execute(robot)){
            return "True";
        } else {
            return "False";
        }
    }
};

#endif