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
        string portStr = port->executeString(robot);
        if (portStr.empty()) return false;
        string port_name = string(1, toupper((unsigned char)portStr[0]));

        if (robot.distance_states.find(port_name) == robot.distance_states.end()) return false;
        auto ds = robot.distance_states[port_name];
        if (!ds) return false;

        string distStr = distance->executeString(robot);
        if (!is_number(distStr)) return false;
        double target = distance->execute(robot);

        if (ds->value == ds->previous_value) return false;

        return check_distance(ds->value, option, target);
    }
};

#endif