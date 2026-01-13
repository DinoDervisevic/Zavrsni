#ifndef IS_DISTANCE_HPP
#define IS_DISTANCE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class IsDistance : public Block {
    Block* port;
    string option;
    Block* distance;
    string unit;
public:
    IsDistance(Block* port, string comparator, Block* value, string unit) : Block("Sensor", "IsDistance"), port(port), option(comparator), distance(value), unit(unit) {}

    double execute(Robot& robot) override {
        string portStr = port->executeString(robot);
        if (portStr.empty()) return 0;
        string port_name = string(1, toupper((unsigned char)portStr[0]));

        if (robot.distance_states.find(port_name) == robot.distance_states.end()) return 0;
        auto ds = robot.distance_states[port_name];
        if (!ds) return 0;

        string distStr = distance->executeString(robot);
        if (!is_number(distStr)) return 0;
        double target = distance->execute(robot);

        if (ds->value == ds->previous_value) return 0;

        if (check_distance(ds->value, option, target)) {
            return 1;
        } else {
            return 0;
        }
    }

    string executeString(Robot& robot) override {
        string portStr = port->executeString(robot);
        if (portStr.empty()) return "False";
        string port_name = string(1, toupper((unsigned char)portStr[0]));

        if (robot.distance_states.find(port_name) == robot.distance_states.end()) return "False";
        auto ds = robot.distance_states[port_name];
        if (!ds) return "False";

        string distStr = distance->executeString(robot);
        if (!is_number(distStr)) return "False";
        double target = distance->execute(robot);

        if (ds->value == ds->previous_value) return "False";

        if (check_distance(ds->value, option, target)) {
            return "True";
        } else {
            return "False";
        }
    }
};

#endif