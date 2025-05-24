#ifndef RESET_YAW_ANGLE_HPP
#define RESET_YAW_ANGLE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class ResetYawAngle : public Block {
public:
    ResetYawAngle() : Block("Sensor", "ResetYawAngle") {}

    double execute(Robot& robot) override {
        robot.default_yaw_angle = robot.angle;
        return 0;
    }
};

#endif