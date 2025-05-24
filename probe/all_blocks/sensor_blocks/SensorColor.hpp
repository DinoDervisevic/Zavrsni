#ifndef SENSOR_COLOR_HPP
#define SENSOR_COLOR_HPP

#include "../common_block_includes.hpp"

using namespace std;

class SensorColor : public Block {
    Block* port;
public:
    SensorColor(Block* port) : Block("Sensor", "SensorColor"), port(port) {}

    double execute(Robot& robot) override {
        if(port->executeString(robot).length() < 1) return 0;
        string port_name = string(1, toupper(port->executeString(robot)[0]));
        if (robot.color_states.find(port_name) != robot.color_states.end()){
            return robot.color_states[port_name]->value;
        }
    }
};

#endif