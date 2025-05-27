#ifndef WHEN_COLOR_HPP
#define WHEN_COLOR_HPP

#include "../common_block_includes.hpp"

using namespace std;

class WhenColor : public Block {
    Block* port;
    Block* color;
public:
    WhenColor(Block* port, Block* color) : Block("Event", "WhenColor"), port(port), color(color) {}

    double execute(Robot& robot) override {
        return 0;
    }

    bool done(Robot& robot) override {
        if(port->executeString(robot).length() < 1) return false;
        string port_name = string(1, toupper(port->executeString(robot)[0]));
        if (robot.color_states.find(port_name) != robot.color_states.end() 
        && is_number(color->executeString(robot))
        && robot.color_states[port_name]->value == color->execute(robot)
        && robot.color_states[port_name]->value != robot.color_states[port_name]->previous_value) {
            return true;
        }
        else return false;
    }
};

#endif