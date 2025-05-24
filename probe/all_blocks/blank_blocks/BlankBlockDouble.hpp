#ifndef BLANK_BLOCK_DOUBLE_HPP
#define BLANK_BLOCK_DOUBLE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class BlankBlockDouble : public Block {
    double value;
public:
    BlankBlockDouble(double value) : Block("Blank", "BlankBlockDouble"), value(value) {}

    double execute(Robot& robot) override {
        return value;
    }
};

#endif