#ifndef ROBOT_H
#define ROBOT_H

#include <string>
#include <map>

using namespace std;

struct State {
    string device_type;
    double value;

    State(string device_type, double value) : device_type(device_type), value(value) {}

    virtual ~State() = default;
};

struct MotorState : public State {
    //double duration;
    double position = 0;

    MotorState(double speed) : State("Motor", speed) {}
};

struct ColorSensor : public State {

    ColorSensor(double value) : State("ColorSensor", value){}
};

struct DistanceSensor : public State {

    DistanceSensor(double value) : State("DistanceSensor", value){}
};

struct ForceSensor : public State {

    double previous_value = 0;

    ForceSensor(double value) : State("ForceSensor", value){}
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
    double pixel_display[5][5] = {0}; // 5x5 matrix of pixel brightness
    map<string, State*> states; // motor name -> motor state
    string movement_motors[2] = {"", ""}; // names of the motors
    pair<double, string> motor_rotation = {0, "cm"}; // direction of the movement

    int discrete_time_interval = 0.05; // time in seconds between each simulation step

    Robot(string name, int x, int y) : name(name), x(x), y(y) {}

    ~Robot() {
        for (auto& pair : states) {
            delete pair.second;
        }
    }

    void addMotorState(const string& key, double speed, double duration) {
        states[key] = new MotorState(speed);
    }

    void addColorSensor(const string& key, double value) {
        states[key] = new ColorSensor( value);
    }

    void addDistanceSensor(const string& key, double value) {
        states[key] = new DistanceSensor(value);
    }

    void addForceSensor(const string& key, double value) {
        states[key] = new ForceSensor(value);
    }
};

#endif // ROBOT_H