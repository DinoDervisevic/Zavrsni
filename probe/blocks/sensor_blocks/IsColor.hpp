#ifndef IS_COLOR_HPP
#define IS_COLOR_HPP

#include "../common_block_includes.hpp"

using namespace std;

class IsColor : public Block {
    Block* port;
    Block* color;
public:
    IsColor(Block* port, Block* color) : Block("Sensor", "IsColor"), port(port), color(color) {}

    double execute(Robot& robot) override {
        if(port->executeString(robot).length() < 1) return 0;
        string port_name = string(1, toupper(port->executeString(robot)[0]));
        if (robot.color_states.find(port_name) != robot.color_states.end() 
        && robot.color_states[port_name]->value == color->execute(robot)) {
            return 1;
        }
        else return 0;
    }

    string executeString(Robot& robot) override {
        if(port->executeString(robot).length() < 1) return 0;
        string port_name = string(1, toupper(port->executeString(robot)[0]));
        if (robot.color_states.find(port_name) != robot.color_states.end() 
        && robot.color_states[port_name]->value == color->execute(robot)) {
            return "True";
        }
        else return "False";
    }
};

#endif