#ifndef MOTOR_SET_SPEED_HPP
#define MOTOR_SET_SPEED_HPP

#include "../common_block_includes.hpp"

using namespace std;

class MotorSetSpeed : public Block {
    Block* port;
    Block* speed;
public:
    MotorSetSpeed(Block* port, Block* speed) : Block("Motor", "MotorSetSpeed"), port(port), speed(speed) {}

    double execute(Robot& robot) override { 
        string good_ports = parse_port(robot, port->executeString(robot), "Motor");
        if(good_ports == "") return 0;
        if(!is_number(speed->executeString(robot))) return 0;
        for(int i = 0; i < good_ports.length(); ++i){
            if(robot.motor_states.find(string(1, good_ports[i])) != robot.motor_states.end()){
                robot.motor_states[string(1, good_ports[i])]->speed = speed->execute(robot);
            }
        }
        return 0;
    }
};

#endif