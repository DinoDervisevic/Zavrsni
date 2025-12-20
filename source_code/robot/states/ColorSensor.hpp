#ifndef COLORSENSOR_HPP
#define COLORSENSOR_HPP

#include "State.hpp"

using namespace std;

struct ColorSensor : public State {
    double previous_value = -1;

    ColorSensor(double value) : State("ColorSensor", value){}
};

#endif