#ifndef WHEN_PRESSED_HPP
#define WHEN_PRESSED_HPP

#include "../common_block_includes.hpp"

using namespace std;

class WhenPressed : public Block {
    Block* port;
    string event;
public:
    WhenPressed(Block* port, string event) : Block("Event", "WhenPressed"), port(port), event(event) {}

    double execute(Robot& robot) override {
        return 0;
    }

    bool done(Robot& robot) override {
        if(port->executeString(robot).length() < 1) return false;
        string port_name = string(1, toupper(port->executeString(robot)[0]));
        if (robot.force_states.find(port_name) != robot.force_states.end()
        && calculate_pressed_event(static_cast<ForceSensor*>(robot.force_states[port_name]), event)) {
            return true;
        }
        else return false;
    }
};

#endif