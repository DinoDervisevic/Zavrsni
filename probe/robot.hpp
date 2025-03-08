#ifndef ROBOT_H
#define ROBOT_H

#include <string>

struct Robot {
    std::string name;
    int x, y;
    double v1, v2;
    int angle;
    int wheel_distance;
    int wheel_radius;
    std::string sound_state, display_state, button_color_state;
    std::string sensor_color;
    double sensor_distance;
    Robot(std::string name, int x, int y) : name(name), x(x), y(y) {}
};

#endif // ROBOT_H