#ifndef PICK_RANDOM_HPP
#define PICK_RANDOM_HPP

#include "../common_block_includes.hpp"
#include <random>

using namespace std;

class PickRandom : public Block {
    Block* from;
    Block* to;
public:
    PickRandom(Block* from, Block* to) : Block("Operator", "PickRandom"), from(from), to(to) {}

    double execute(Robot& robot) override {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(from->execute(robot), to->execute(robot));
        return dis(gen);
    }
};

#endif