#ifndef WHEN_DISTANCE_HPP
#define WHEN_DISTANCE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class WhenDistance : public Block {
    Block* distance;
    Block* port;
    string option;
    string unit;
public:
    WhenDistance(Block* port, string option, Block* distance, string unit) : Block("Event", "WhenDistance"), port(port), option(option), distance(distance), unit(unit) {}

    double execute(Robot& robot) override {
        return 0;
    }

    bool done(Robot& robot) override {
        if(port->executeString(robot).length() < 1) return false;
        string port_name = string(1, toupper(port->executeString(robot)[0]));
        if (robot.distance_states.find(port_name) != robot.distance_states.end()
        && is_number(distance->executeString(robot))
        && robot.distance_states[port_name]->value != robot.distance_states[port_name]->previous_value
        && check_distance(robot.distance_states[port_name]->value == distance->execute(robot), option, distance->execute(robot))) {
            return true;
        }
        else return false;
    }
};

#endif