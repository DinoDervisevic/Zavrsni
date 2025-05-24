#ifndef DISTANCESENSOR_HPP
#define DISTANCESENSOR_HPP

#include "State.hpp"

using namespace std;

struct DistanceSensor : public State {
    double previous_value = 0;

    DistanceSensor(double value) : State("DistanceSensor", value){}
};

#endif