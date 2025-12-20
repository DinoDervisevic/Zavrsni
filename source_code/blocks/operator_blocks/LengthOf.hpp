#ifndef LENGTH_OF_HPP
#define LENGTH_OF_HPP

#include "../common_block_includes.hpp"

using namespace std;

class LengthOf : public Block {
    Block* value;
public:
    LengthOf(Block* value) : Block("Operator", "LengthOf"), value(value) {}

    double execute(Robot& robot) override {
        return value->executeString(robot).length();
    }
};

#endif