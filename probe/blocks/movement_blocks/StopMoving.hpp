#ifndef STOP_MOVING_HPP
#define STOP_MOVING_HPP

#include "../common_block_includes.hpp"

using namespace std;

class StopMoving : public Block {
public:
    StopMoving() : Block("Move", "StopMoving") {}

    double execute(Robot& robot) override {
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.movement_motors[0] == robot.movement_motors[1]) {
            return 0;
        }

        robot.motor_states[robot.movement_motors[0]] -> value = 0;
        robot.motor_states[robot.movement_motors[1]] -> value = 0;

        robot.motor_states[robot.movement_motors[0]] -> time_left = 0;
        robot.motor_states[robot.movement_motors[1]] -> time_left = 0;

        return 0;
    }

    void finish(Robot& robot) override {
        robot.movement_block_in_effect = false;
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