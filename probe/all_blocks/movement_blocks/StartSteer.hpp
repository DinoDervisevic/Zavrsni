#ifndef START_STEER_HPP
#define START_STEER_HPP

#include "../common_block_includes.hpp"

using namespace std;

class StartSteer : public Block {
    Block* direction;

public:
    StartSteer(Block* direction) : Block("Move", "StartSteer"), direction(direction) {}

    double execute(Robot& robot) override { // TODO : tu isto direction je jedan veliki ?
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.movement_motors[0] == robot.movement_motors[1]) {
            return 0;
        }

        robot.motor_states[robot.movement_motors[0]] -> value = (-1) * robot.movement_speed * min(1 + direction->execute(robot)/50, 1.0);
        robot.motor_states[robot.movement_motors[1]] -> value = robot.movement_speed * min(1 - direction->execute(robot)/50, 1.0);

        robot.motor_states[robot.movement_motors[0]]->time_left = 31536000; // the motors will run for a year
        robot.motor_states[robot.movement_motors[1]]->time_left = 31536000; // the motors will run for a year

        robot.movement_block_in_effect = true;
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