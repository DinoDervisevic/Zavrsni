#ifndef ROBOT_STATE_HPP
#define ROBOT_STATE_HPP

#include <string>
#include <map>

#include "../states/states_include.hpp"

using namespace std;

// Class to represent the robot's state; will be expanded later to fit more complex functions
struct RobotState {
    double x, y, t;
    int angle;
    string sound_playing;

    double pixel_display[5][5] = {0};
    double permanent_pixel_display[5][5] = {0};

    map<string, double> motor_states;
    map<string, double> motor_states_value;
    map<string, double> motor_states_speed;
    map<string, double> color_states;
    map<string, double> distance_states;
    map<string, double> force_states;

    RobotState(
        double x, double y, double t, int angle, const string& sound_playing,
        const double pixel_display_[5][5],
        const double permanent_pixel_display_[5][5],
        const map<string, MotorState*>& motor_states,
        const map<string, ColorSensor*>& color_states,
        const map<string, DistanceSensor*>& distance_states,
        const map<string, ForceSensor*>& force_states
    )
        : x(x), y(y), t(t), angle(angle), sound_playing(sound_playing)
    {
        // Kopiraj 5x5 polja
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 5; ++j) {
                pixel_display[i][j] = pixel_display_[i][j];
                permanent_pixel_display[i][j] = permanent_pixel_display_[i][j];
            }
        
        for(const auto& pair : motor_states) {
            this->motor_states[pair.first] = pair.second->position;
        }
        for(const auto& pair : motor_states) {
            this->motor_states_speed[pair.first] = pair.second->speed;
        }
        for(const auto& pair : motor_states) {
            this->motor_states_value[pair.first] = pair.second->value;
        }
        for(const auto& pair : color_states) {
            this->color_states[pair.first] = pair.second->value;
        }
        for(const auto& pair : distance_states) {
            this->distance_states[pair.first] = pair.second->value;
        }
        for(const auto& pair : force_states) {
            this->force_states[pair.first] = pair.second->value;
        }
    }

};

#endif