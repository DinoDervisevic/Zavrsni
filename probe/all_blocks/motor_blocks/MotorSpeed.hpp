#ifndef MOTOR_SPEED_HPP
#define MOTOR_SPEED_HPP

#include "../common_block_includes.hpp"

using namespace std;

class MotorSpeed : public Block {
    Block* port;
public:
    MotorSpeed(Block* port) : Block("Motor", "MotorSpeed"), port(port) {}

    double execute(Robot& robot) override {
        if(port == nullptr || port->executeString(robot) == ""){
            return 0;
        }
        if(robot.motor_states.find(string(1, toupper(port->executeString(robot)[0]))) != robot.motor_states.end()){
            return robot.motor_states[string(1, toupper(port->executeString(robot)[0]))]->speed;
        }
        return 0;
    }
};

#endif
