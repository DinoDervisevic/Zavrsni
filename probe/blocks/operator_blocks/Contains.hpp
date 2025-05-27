#ifndef CONTAINS_HPP
#define CONTAINS_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Contains : public Block {
    Block* value1;
    Block* value2;
public:
    Contains(Block* value1, Block* value2) : Block("Operator", "Contains"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->executeString(robot).find(value2->executeString(robot)) != string::npos;
    }

    string executeString(Robot& robot) override {
        if(value1->executeString(robot).find(value2->executeString(robot)) != string::npos){
            return "True";
        } else {
            return "False";
        }
    }
};

#endif