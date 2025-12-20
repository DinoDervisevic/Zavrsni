#ifndef STATE_HPP
#define STATE_HPP

#include <string>

using namespace std;

struct State {
    string device_type;
    double value;

    State(string device_type, double value) : device_type(device_type), value(value) {}

    virtual ~State() = default;
};

#endif