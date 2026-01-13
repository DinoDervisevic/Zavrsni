#ifndef SENSOR_DISTANCE_HPP
#define SENSOR_DISTANCE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class SensorDistance : public Block {
    Block* port;
    string unit;
public:
    SensorDistance(Block* port, string unit) : Block("Sensor", "SensorDistance"), port(port), unit(unit) {}

    double execute(Robot& robot) override {
        string portStr = port->executeString(robot);
        if (portStr.empty()) return 0;
        string port_name = string(1, toupper((unsigned char)portStr[0]));
        if (robot.distance_states.find(port_name) != robot.distance_states.end() && robot.distance_states[port_name]) {
            return robot.distance_states[port_name]->value;
        }
        return 0;
    }
};

#endif