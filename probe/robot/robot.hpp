#ifndef ROBOT_H
#define ROBOT_H

#include <string>
#include <map>
#include <cmath>
#include <vector>

#include "robot_include.hpp"

using namespace std;

class BlockSequence;

struct Robot {
    string name;
    double x, y = 0;
    int angle = 0;
    int default_yaw_angle = 0; // when calculating yaw angle, this needs to be subtracted from the current angle
    int pitch_angle = 0;
    int roll_angle = 0;
    double wheel_distance = 6.8; // distance between the wheels in cm
    double wheel_radius = 5.55/2; // radius of the wheel in cm
    double movement_speed = 50;
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

    string movement_motors[2] = {"A", "B"}; // names of the motors
    bool movement_block_in_effect = false; // keeps track of wether the robot is moving due to the movement block or motor block(s)

    pair<double, string> motor_rotation = {0, "cm"}; // direction of the movement

    double time_since_start = 0; // current time in seconds
    double discrete_time_interval = 0.005; // time in seconds between each simulation step

    vector<string> broadcasts; // list of broadcasted messages
    vector<string> finished_broadcasts; // list of finished broadcasts

    vector<BlockSequence*> block_sequences; // list of all block sequences

    MotionVector motion_vector = {0, 0}; // linear and angular velocity

    vector<RobotState> robot_states; // list of all robot states

    void save_state() {
        RobotState state(x, y, time_since_start, angle, sound_playing, pixel_display, permanent_pixel_display, 
            motor_states, color_states, distance_states, force_states);
        robot_states.push_back(state);
    }

    double calculate_wheel_speed(string port){
        double speed = 0;
        if(movement_block_in_effect && (port == movement_motors[0] || port == movement_motors[1])){
            speed = motor_states[port]->current_speed / 100 / 0.387;
        } else if (motor_states.find(port) != motor_states.end()) {
            speed = motor_states[port]->current_speed / 100 / 0.387;	
        }
        return wheel_radius * 2 * 3.14159265358979323846 * speed;
    }

    void calculate_motor_position(string port){
        if(motor_states.find(port) != motor_states.end()){
            motor_states[port]->position += 360 * discrete_time_interval * (motor_states[port]->current_speed / 100) / 0.387;
            if(motor_states[port]->position >= 360){
                motor_states[port]->position -= 360;
            } else if(motor_states[port]->position < 0){
                motor_states[port]->position += 360;
            }
        }
    }

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
            motor_states[movement_motors[i]]->time_left = 0;
        }
    }

    void reset() {
        for (auto& pair : motor_states) {
            pair.second->value = 0;
            pair.second->time_left = 0;
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

    void addMotorState(const string& key, double speed) {
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




    void print_position() {
        cout << "Robot position: (" << x << ", " << y << "), angle: " << angle << endl;
    }

    void print_display() {
        cout << "Pixel display: " << endl;
        if(is_permanent_display){
            for (int i = 0; i < 5; ++i) {
                for (int j = 0; j < 5; ++j) {
                    cout << permanent_pixel_display[i][j] << " ";
                }
                cout << endl;
            }
        }
        else{
            for (int i = 0; i < 5; ++i) {
                for (int j = 0; j < 5; ++j) {
                    cout << pixel_display[i][j] << " ";
                }
                cout << endl;
            }
        }
    }

    void print_sound() {
        cout << "Sound: " << sound_playing << endl;
    }
};

#endif // ROBOT_H