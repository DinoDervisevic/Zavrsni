#ifndef MOTOR_POSITION_HPP
#define MOTOR_POSITION_HPP

#include "../common_block_includes.hpp"

using namespace std;

class MotorPosition : public Block {
    Block* port;
public:
    MotorPosition(Block* port) : Block("Motor", "MotorPosition"), port(port) {}

    double execute(Robot& robot) override {
        if(port == nullptr || port->executeString(robot) == ""){
            return 0;
        }
        if(robot.motor_states.find(string(1, toupper(port->executeString(robot)[0]))) != robot.motor_states.end()){
            return robot.motor_states[string(1, toupper(port->executeString(robot)[0]))]->position;
        }
        return 0;
    }
};

#endif