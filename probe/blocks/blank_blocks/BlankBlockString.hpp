#ifndef BLANK_BLOCK_STRING_HPP
#define BLANK_BLOCK_STRING_HPP

#include "../common_block_includes.hpp"

using namespace std;

class BlankBlockString : public Block {
    string value;
public:
    BlankBlockString(string value) : Block("Blank", "BlankBlockString"), value(value) {}

    double execute(Robot& robot) override {
        return 0;
    }

    string executeString(Robot& robot) override {
        return value;
    }
};

#endif