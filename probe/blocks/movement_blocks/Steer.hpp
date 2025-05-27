#ifndef STEER_HPP
#define STEER_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Steer : public Block {
    Block* direction;
    Block* value;
    string unit;

public:
    Steer(Block* direction, Block* value, string unit) : Block("Move", "Steer"), direction(direction), value(value), unit(unit) {}

    double execute(Robot& robot) override { // TODO : nemma pojma sto radit s direction --> skuzi i popravi
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.movement_motors[0] == robot.movement_motors[1]
        || !is_number(value->executeString(robot))
        || !is_number(direction->executeString(robot))) {
            return 0;
        }

        robot.motor_states[robot.movement_motors[0]] -> value = (-1) * robot.movement_speed * min(1 + direction->execute(robot)/50, 1.0);
        robot.motor_states[robot.movement_motors[1]] -> value = robot.movement_speed * min(1 - direction->execute(robot)/50, 1.0);
        
        robot.movement_block_in_effect = true;
        double time = convert_to_seconds_movement(robot, unit, value->execute(robot));
        robot.motor_states[robot.movement_motors[0]] -> time_left = time;
        robot.motor_states[robot.movement_motors[1]] -> time_left = time;
        return time;
    }

    void finish(Robot& robot) override {
        robot.movement_block_in_effect = false;
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.movement_motors[0] == robot.movement_motors[1]
        || !is_number(value->executeString(robot))) {
            return;
        }
        robot.motor_states[robot.movement_motors[0]] -> value = 0;
        robot.motor_states[robot.movement_motors[1]] -> value = 0;
        robot.motor_states[robot.movement_motors[0]] -> time_left = 0;
        robot.motor_states[robot.movement_motors[1]] -> time_left = 0;
        return;
    }

    void deal_with_interference(Robot& robot, BlockSequence* sequence) override {
        Block* block = sequence->get_current_block();
        if (block->name == "Move" || block->name == "Steer") {
            block->finish(robot);
            sequence->set_time_left(0);
        }
        if (block->name == "MotorTurnForDirection" || block->name == "MotorGoDirectionToPosition"){
            block->stop_motors(robot, robot.movement_motors[0]);
            block->stop_motors(robot, robot.movement_motors[1]);
        }
    }
};

#endif