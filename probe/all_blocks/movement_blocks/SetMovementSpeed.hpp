#ifndef SET_MOVEMENT_SPEED_HPP
#define SET_MOVEMENT_SPEED_HPP

#include "../common_block_includes.hpp"

using namespace std;

class SetMovementSpeed : public Block {
    Block* speed;
public:
    SetMovementSpeed(Block* speed) : Block("Move", "SetMovementSpeed"), speed(speed) {}

    double execute(Robot& robot) override {
        robot.movement_speed = speed->execute(robot);
        return 0;
    }
};

#endif