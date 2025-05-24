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
        if(port->executeString(robot).length() < 1) return 0;
        string port_name = string(1, toupper(port->executeString(robot)[0]));
        if (robot.distance_states.find(port_name) != robot.distance_states.end()) {
            return robot.distance_states[port_name]->value; // TODO: ovdje i gore skuziit sto tocno unti znaci i kako se radi s postotcima
        }
    }
};

#endif