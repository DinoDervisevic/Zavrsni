#ifndef SENSOR_FORCE_HPP
#define SENSOR_FORCE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class SensorForce : public Block {
    Block* port;
    string unit;
public:
    SensorForce(Block* port, string unit) : Block("Sensor", "SensorForce"), port(port), unit(unit) {}

    double execute(Robot& robot) override {
        if(port->executeString(robot).length() < 1) return 0;
        string port_name = string(1, toupper(port->executeString(robot)[0]));
        if (robot.force_states.find(port_name) != robot.force_states.end()){
            return robot.force_states[port_name]->value * (unit == "%" ? 100 : 1); 
        }
        else return 0;
    }
};

#endif