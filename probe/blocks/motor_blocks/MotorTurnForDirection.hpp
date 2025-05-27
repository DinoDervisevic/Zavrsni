#ifndef MOTOR_TURN_FOR_DIRECTION_HPP
#define MOTOR_TURN_FOR_DIRECTION_HPP

#include "../common_block_includes.hpp"

using namespace std;

class MotorTurnForDirection : public Block {
    Block* port;
    bool forward;
    Block* value;
    string unit;

    string good_ports = "";
    map<string, double> motor_times;

    bool first_time = true;
public:
    MotorTurnForDirection(Block* port, Block* value, bool forward, string unit) : Block("Motor", "MotorTurnForDirection"), port(port), value(value), forward(forward), unit(unit) {}

    double execute(Robot& robot) override { 
        if(!is_number(value->executeString(robot))) return 0;
        if(first_time){
            if(port == nullptr || port->executeString(robot) == ""){
                return 0;
            }
            helper(robot);
            first_time = false;
        }
        string good_ports2 = good_ports;

        if(good_ports == "") return 0;
        
        for(int i = 0; i < good_ports.length(); ++i){
            if(motor_times[string(1, good_ports[i])] <= 0){
                robot.motor_states[string(1, good_ports[i])]->value = 0;
                good_ports2.erase(remove(good_ports2.begin(), good_ports2.end(), good_ports[i]), good_ports2.end());
            }
            else motor_times[string(1, good_ports[i])] -= robot.discrete_time_interval;
        }
        good_ports = good_ports2;

        if(good_ports2 == ""){
            return 0;
        }
        return 0;
    }

    void helper(Robot& robot) {
        good_ports = parse_port(robot, port->executeString(robot), "Motor");

        for(int i = 0; i < good_ports.length(); ++i){
            if(robot.motor_states.find(string(1, good_ports[i])) != robot.motor_states.end()){
                robot.motor_states[string(1, good_ports[i])]->value = robot.motor_states[string(1, good_ports[i])]->speed * (forward ? 1 : -1);

                double time = convert_to_seconds_motor(robot, unit, value->execute(robot), string(1, good_ports[i]));
                motor_times[string(1, good_ports[i])] = time;
                robot.motor_states[string(1, good_ports[i])]->time_left = time;
            }
        }
    }

    void finish(Robot& robot) override {
        first_time = true;
        good_ports = "";
        motor_times.clear();
    }

    bool done(Robot& robot){
        return good_ports == "";
    }

    void stop_motors(Robot& robot, string port) override{
        for(int i = 0; i < port.length(); ++i){
            good_ports.erase(remove(good_ports.begin(), good_ports.end(), port[i]), good_ports.end());
        }
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