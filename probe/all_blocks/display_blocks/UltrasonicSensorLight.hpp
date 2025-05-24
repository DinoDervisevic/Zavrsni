#ifndef ULTRASONIC_SENSOR_LIGHT_HPP
#define ULTRASONIC_SENSOR_LIGHT_HPP

#include "../common_block_includes.hpp"

using namespace std;

class UltrasonicSensorLight : public Block { // TODO : kad budem mogao isprobat
    Block* color;
    Block* port;

public:
    UltrasonicSensorLight(Block* color, Block* port) : Block("Display", "UltrasonicSensorLight"), color(color), port(port) {}

    double execute(Robot& robot) override {
        return 0;
    }
};

#endif