#ifndef IS_IN_BETWEEN_HPP
#define IS_IN_BETWEEN_HPP

#include "../common_block_includes.hpp"

using namespace std;

class IsInBetween : public Block {
    Block* value1;
    Block* value2;
    Block* value3;
public:
    IsInBetween(Block* value1, Block* value2, Block* value3) : Block("Operator", "IsInBetween"), value1(value1), value2(value2), value3(value3) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) < value2->execute(robot) && value2->execute(robot) < value3->execute(robot);
    }

    string executeString(Robot& robot) override {
        if(value1->execute(robot) < value2->execute(robot) && value2->execute(robot) < value3->execute(robot)){
            return "True";
        } else {
            return "False";
        }
    }
};

#endif