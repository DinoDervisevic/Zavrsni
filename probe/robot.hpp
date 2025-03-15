#ifndef ROBOT_H
#define ROBOT_H

#include <string>
#include <map>

using namespace std;

struct MotorState {
    double speed;
    double duration;
    string connected_device;
};

struct Robot {
    string name;
    int x, y;
    double v1 = 0, v2 = 0;
    int angle = 0;
    int wheel_distance;
    int wheel_radius;
    int movement_speed;
    double volume;
    string sound_state, button_color_state;
    string sensor_color;
    double sensor_distance;
    double pixel_display[5][5] = {0}; // 5x5 matrix of pixel brightness
    map<string, MotorState> motor_states; // motor name -> motor state
    string left_wheel, right_wheel; // movement motors

    int discrete_time_interval = 0.05; // time in seconds between each simulation step

    Robot(string name, int x, int y) : name(name), x(x), y(y) {}
};

#endif // ROBOT_H