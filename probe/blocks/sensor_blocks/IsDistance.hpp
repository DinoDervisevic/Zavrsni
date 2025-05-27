#ifndef IS_DISTANCE_HPP
#define IS_DISTANCE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class IsDistance : public Block {
    Block* port;
    string comparator;
    Block* value;
    string unit;
public:
    IsDistance(Block* port, string comparator, Block* value, string unit) : Block("Sensor", "IsDistance"), port(port), comparator(comparator), value(value), unit(unit) {}

    double execute(Robot& robot) override {
        if(port->executeString(robot).length() < 1) return 0;
        string port_name = string(1, toupper(port->executeString(robot)[0]));
        if (robot.distance_states.find(port_name) != robot.distance_states.end()
        && robot.distance_states[port_name]->value != robot.distance_states[port->executeString(robot)]->previous_value
        && check_distance(robot.distance_states[port_name]->value == value->execute(robot), comparator, value->execute(robot))) {
            return 1;
        }
        else return 0;
    }

    string executeString(Robot& robot) override {
        if(port->executeString(robot).length() < 1) return 0;
        string port_name = string(1, toupper(port->executeString(robot)[0]));
        if (robot.distance_states.find(port_name) != robot.distance_states.end()
        && robot.distance_states[port_name]->value != robot.distance_states[port->executeString(robot)]->previous_value
        && check_distance(robot.distance_states[port_name]->value == value->execute(robot), comparator, value->execute(robot))) {
            return "True";
        }
        return "False";
    }
};

#endif