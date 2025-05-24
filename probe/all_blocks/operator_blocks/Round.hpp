#ifndef ROUND_HPP
#define ROUND_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Round : public Block {
    Block* value;
public:
    Round(Block* value) : Block("Operator", "Round"), value(value) {}

    double execute(Robot& robot) override {
        return round(value->execute(robot));
    }
};

#endif
