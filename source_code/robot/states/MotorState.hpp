#ifndef MOTOR_STATE_HPP
#define MOTOR_STATE_HPP

#include "State.hpp"

using namespace std;

struct MotorState : public State {
    double position = 0;
    double speed = 75;
    double current_speed = 0;
    double time_left = 0;

    MotorState(double speed) : State("Motor", speed) {
        this->value = 0;
    }
};

#endif