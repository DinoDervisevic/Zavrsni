#ifndef START_MOVE_HPP
#define START_MOVE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class StartMove : public Block {
    bool forward;

public:
    StartMove(bool forward) : Block("Move", "StartMove"), forward(forward) {}

    double execute(Robot& robot) override {
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.movement_motors[0] == robot.movement_motors[1]) {
            return 0;
        }

        if(forward){ // TODO: check if this is correct
            robot.motor_states[robot.movement_motors[0]] -> value = -robot.movement_speed;
            robot.motor_states[robot.movement_motors[1]] -> value = robot.movement_speed;
        } else {
            robot.motor_states[robot.movement_motors[0]] -> value = robot.movement_speed;
            robot.motor_states[robot.movement_motors[1]] -> value = -robot.movement_speed;
        }
        robot.movement_block_in_effect = true;
        robot.motor_states[robot.movement_motors[0]]->time_left = 31536000; // the motors will run for a year
        robot.motor_states[robot.movement_motors[1]]->time_left = 31536000; // the motors will run for a year
        return 0;
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