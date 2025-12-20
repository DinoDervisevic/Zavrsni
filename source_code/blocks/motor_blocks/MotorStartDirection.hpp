#ifndef MOTOR_START_DIRECTION_HPP
#define MOTOR_START_DIRECTION_HPP

#include "../common_block_includes.hpp"

using namespace std;

class MotorStartDirection : public Block {
    Block* port;
    bool forward;
public:
    MotorStartDirection(Block* port, bool forward) : Block("Motor", "MotorStartDirection"), port(port), forward(forward) {}

    double execute(Robot& robot) override { 
        string good_ports = parse_port(robot, port->executeString(robot), "Motor");
        if(good_ports == "") return 0;
        for(int i = 0; i < good_ports.length(); ++i){
            if(robot.motor_states.find(string(1, good_ports[i])) != robot.motor_states.end()){
                robot.motor_states[string(1, good_ports[i])]->value = robot.motor_states[string(1, good_ports[i])]->speed * (forward ? 1 : -1);
                robot.motor_states[string(1, good_ports[i])]->time_left = 31536000; // the motors will run for a year
            }
        }
        return 0;
    }

    void deal_with_interference(Robot& robot, BlockSequence* sequence) override {
        Block* block = sequence->get_current_block();
        string good_ports_help = parse_port(robot, port->executeString(robot), "Motor");
        if (block->name == "Move" || block->name == "Steer") {
            for (int i = 0; i < good_ports_help.length(); ++i){
                if(good_ports_help[i] == robot.movement_motors[0][0] || good_ports_help[i] == robot.movement_motors[1][0]){
                    block->finish(robot);
                    sequence->set_time_left(0);
                    break;
                }
            }
        }
        if (block->name == "MotorTurnForDirection" || block->name == "MotorGoDirectionToPosition"){
            block->stop_motors(robot, good_ports_help);
        }
        if (robot.movement_block_in_effect){
            for (int i = 0; i < good_ports_help.length(); ++i){
                if(good_ports_help[i] == robot.movement_motors[0][0] || good_ports_help[i] == robot.movement_motors[1][0]){
                    robot.movement_block_in_effect = false;
                    robot.reset_movement_motors();
                    break;
                }
            }
        }
    }
};

#endif