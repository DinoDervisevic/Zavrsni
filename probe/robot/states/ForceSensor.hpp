#ifndef FORCE_SENSOR_HPP
#define FORCE_SENSOR_HPP

#include "State.hpp"

using namespace std;

struct ForceSensor : public State {
    double previous_value = 0;

    ForceSensor(double value) : State("ForceSensor", value){}
};

#endif