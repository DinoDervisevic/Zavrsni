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
    double speed = 75;

    MotorState(double speed) : State("Motor", speed) {}
};

struct ColorSensor : public State {
    double previous_value = 0;

    ColorSensor(double value) : State("ColorSensor", value){}
};

struct DistanceSensor : public State {
    double previous_value = 0;

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
    int default_yaw_angle = 0; // when calculating yaw angle, this needs to be subtracted from the current angle
    int pitch_angle = 0;
    int roll_angle = 0;
    int wheel_distance;
    int wheel_radius;
    int movement_speed = 50;
    double volume;
    string sound_state;
    int button_color;

    double pixel_display[5][5] = {0}; // 5x5 matrix of pixel brightness
    double permanent_pixel_display[5][5] = {0}; // 5x5 matrix of pixel brightness
    bool is_permanent_display = false; // if true, the display will not be reset when the robot is reset
    double pixel_display_brightness = 100; // brightness of the display
    int absolute_image_position = 0;

    string sound_playing = "";

    int tilt_angle = 0; 
    string orientation = ""; // "front", "back", "left side", "right side", "top", "bottom"
    string gesture = ""; // "shaken", "tapped", "falling"

    map<string, string> buttons = {
        {"left", "released"},
        {"right", "released"}
    };

    map<string, MotorState*> motor_states; 
    map<string, ColorSensor*> color_states; 
    map<string, DistanceSensor*> distance_states; 
    map<string, ForceSensor*> force_states; 

    string movement_motors[2] = {"", ""}; // names of the motors
    bool movement_block_in_effect = false; // keeps track of wether the robot is moving due to the movement block or motor block(s)

    pair<double, string> motor_rotation = {0, "cm"}; // direction of the movement

    double time_since_start = 0; // current time in seconds
    double discrete_time_interval = 0.05; // time in seconds between each simulation step

    vector<string> broadcasts; // list of broadcasted messages
    vector<string> finished_broadcasts; // list of finished broadcasts

    vector<BlockSequence*> block_sequences; // list of all block sequences

    Robot(string name, int x, int y, vector<BlockSequence*> block_sequences) : name(name), x(x), y(y), block_sequences(block_sequences) {}

    ~Robot() {
        for (auto& pair : motor_states) {
            delete pair.second;
        }
        for (auto& pair : color_states) {
            delete pair.second;
        }
        for (auto& pair : distance_states) {
            delete pair.second;
        }
        for (auto& pair : force_states) {
            delete pair.second;
        }
    }

    void reset_movement_motors() {
        for (int i = 0; i < 2; ++i) {
            motor_states[movement_motors[i]]->value = 0;
        }
    }

    void reset() {
        for (auto& pair : motor_states) {
            pair.second->value = 0;
        }

        //TODO: This works fine, but only if the display is not permanent, if it is, then reseting the robot should not turn it off
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                pixel_display[i][j] = 0;
            }
        }

        sound_playing = "";
        sound_state = "";
    }

    void changeDiscreteTimeInterval(double new_time_interval) {
        discrete_time_interval = new_time_interval;
    }

    void addMotorState(const string& key, double speed, double duration) {
        motor_states[key] = new MotorState(speed);
    }

    void addColorSensor(const string& key, double value) {
        color_states[key] = new ColorSensor(value);
    }

    void addDistanceSensor(const string& key, double value) {
        distance_states[key] = new DistanceSensor(value);
    }

    void addForceSensor(const string& key, double value) {
        force_states[key] = new ForceSensor(value);
    }
};

#endif // ROBOT_H