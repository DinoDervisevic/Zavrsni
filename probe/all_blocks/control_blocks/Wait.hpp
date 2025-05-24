#ifndef WAIT_HPP
#define WAIT_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Wait : public Block {
    Block* time;
public:
    Wait(Block* time) : Block("Control", "Wait"), time(time) {}

    double execute(Robot& robot) override {
        return time->execute(robot);
    }
};

#endif