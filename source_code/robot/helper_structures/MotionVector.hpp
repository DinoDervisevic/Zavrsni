#ifndef MOTION_VECTOR_HPP
#define MOTION_VECTOR_HPP

#include <string>

using namespace std;

struct MotionVector {
    double linear_velocity;
    double angular_velocity;

    double acceleration = 6.42;

    MotionVector(double linear_velocity, double angular_velocity) : linear_velocity(linear_velocity), angular_velocity(angular_velocity) {}
};

#endif