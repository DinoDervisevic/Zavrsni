#ifndef SET_MOVEMENT_PAIR_HPP
#define SET_MOVEMENT_PAIR_HPP

#include "../common_block_includes.hpp"

using namespace std;

class SetMovementPair : public Block {
    Block* pair;
public:
    SetMovementPair(Block* pair) : Block("Move", "SetMovementPair"), pair(pair) {}
    double execute(Robot& robot) override { 
        if (pair->executeString(robot).length() == 2){
            robot.movement_motors[0] = string(1, toupper(pair->executeString(robot)[0]));
            robot.movement_motors[1] = string(1, toupper(pair->executeString(robot)[1]));
        } 
        else{
            robot.movement_motors[0] = "A";
            robot.movement_motors[1] = "B";
        }
        return 0;
    }
};

#endif