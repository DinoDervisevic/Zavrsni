#ifndef NOT_HPP
#define NOT_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Not : public Block {
    Block* value;
public:
    Not(Block* value) : Block("Operator", "Not"), value(value) {}

    double execute(Robot& robot) override {
        return !value->execute(robot);
    }

    string executeString(Robot& robot) override {
        if(!value->execute(robot)){
            return "True";
        } else {
            return "False";
        }
    }
};

#endif