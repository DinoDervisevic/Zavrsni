#ifndef IS_PRESSED_HPP
#define IS_PRESSED_HPP

#include "../common_block_includes.hpp"

using namespace std;

class IsPressed : public Block {
    Block* port;
    string operation;
public:
    IsPressed(Block* port, string operation) : Block("Sensor", "IsPressed"), port(port), operation(operation) {}

    double execute(Robot& robot) override {
        if(port->executeString(robot).length() < 1) return 0;
        string port_name = string(1, toupper(port->executeString(robot)[0]));
        if (robot.force_states.find(port_name) != robot.force_states.end()
        && calculate_pressed_event(static_cast<ForceSensor*>(robot.force_states[port_name]), operation)) {
            return 1;
        }
        else return 0;
    }

    string executeString(Robot& robot) override {
        if(port->executeString(robot).length() < 1) return 0;
        string port_name = string(1, toupper(port->executeString(robot)[0]));
        if (robot.force_states.find(port_name) != robot.force_states.end()
        && calculate_pressed_event(static_cast<ForceSensor*>(robot.force_states[port_name]), operation)) {
            return "True";
        }
        return "False";
    }
};

#endif