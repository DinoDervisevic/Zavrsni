#ifndef SENSOR_REFLECTIVITY_HPP
#define SENSOR_REFLECTIVITY_HPP

#include "../common_block_includes.hpp"

using namespace std;

class SensorReflectivity : public Block {
    Block* port;
public:
    SensorReflectivity(Block* port) : Block("Sensor", "SensorReflectivity"), port(port) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement reflectivity detection
    }
};

#endif