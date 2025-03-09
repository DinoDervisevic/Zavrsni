#ifndef ROBOT_H
#define ROBOT_H

#include <string>

struct Robot {
    std::string name;
    int x, y;
    double v1 = 0, v2 = 0;
    int angle = 0;
    int wheel_distance;
    int wheel_radius;
    int movement_speed;
    double volume;
    std::string sound_state, button_color_state;
    std::string sensor_color;
    double sensor_distance;
    double pixel_display[5][5] = {0};

    Robot(std::string name, int x, int y) : name(name), x(x), y(y) {}
};

#endif // ROBOT_H