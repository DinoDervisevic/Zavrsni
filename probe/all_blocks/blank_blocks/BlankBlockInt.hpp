#ifndef BLANK_BLOCK_INT_HPP
#define BLANK_BLOCK_INT_HPP

#include "../common_block_includes.hpp"

using namespace std;

class BlankBlockInt : public Block {
    int value;
public:
    BlankBlockInt(int value) : Block("Blank", "BlankBlockInt"), value(value) {}

    double execute(Robot& robot) override {
        return value;
    }
};

#endif