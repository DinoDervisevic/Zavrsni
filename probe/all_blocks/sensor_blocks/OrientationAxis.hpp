#ifndef ORIENTATION_AXIS_HPP
#define ORIENTATION_AXIS_HPP

#include "../common_block_includes.hpp"

using namespace std;

class OrientationAxis : public Block {
    string axis;
public:
    OrientationAxis(string axis) : Block("Sensor", "OrientationAxis"), axis(axis) {}

    double execute(Robot& robot) override {
        if(axis == "yaw") return robot.angle - robot.default_yaw_angle;
        else if(axis == "pitch") return robot.pitch_angle;
        else if(axis == "roll") return robot.roll_angle;
    }
};

#endif