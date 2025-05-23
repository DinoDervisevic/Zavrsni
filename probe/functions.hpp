#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include "json.hpp"
#include "robot.hpp"

using namespace std;

//converts the value of units to seconds
double convert_to_seconds_movement(Robot& robot, string unit, double value) {
    if(unit == "seconds") return value;
    else if(unit == "cm"){
        value = value / (2*3.14159265358979323846*robot.wheel_radius);
        //it takes 0.39 seconds to make 1 rotation, and the acceleration and deceleration take about 0.35 seconds when recahing max speed
        //return value * 0.393 / (robot.movement_speed / 100) + (min(0.34, 0.34 * value / (robot.movement_speed / 100))) * (robot.movement_speed / 100);
    }
    else if(unit == "in"){
        value = value*2.54 / (2*3.14159265358979323846*robot.wheel_radius);
        //return value * 0.393 / (robot.movement_speed / 100) + (min(0.34, 0.34 * value / (robot.movement_speed / 100))) * (robot.movement_speed / 100);
    }
    else if(unit == "rotations"){
        value = value;
        //cout << value * 0.393 / (robot.movement_speed / 100) + (min(0.34, 0.34 * value / (robot.movement_speed / 100))) * (robot.movement_speed / 100) << endl;
        //return value * 0.393 / (robot.movement_speed / 100) + (min(0.34, 0.34 * value / (robot.movement_speed / 100))) * (robot.movement_speed / 100);
    }
    else if(unit == "degrees"){
        value = value/360;
        //return value * 0.393 / (robot.movement_speed / 100) + (min(0.34, 0.34 * value / (robot.movement_speed / 100))) * (robot.movement_speed / 100);
    }
    else value = 0;
    //cout << "Value: " << value << endl;
    double v_max = (robot.movement_speed / 100) / 0.387;
    double s_acc = v_max * v_max / (2 * robot.motion_vector.acceleration);

    double t_total = 0;
    if (value > 2 * s_acc) {
        // Trapezni profil (ima dio s max brzinom)
        t_total = 2 * (v_max / robot.motion_vector.acceleration) + (value - 2 * s_acc) / v_max;
    } else {
        // Trokutasti profil (nikad ne dosegne max brzinu)
        t_total = 2 * sqrt(value / robot.motion_vector.acceleration);
    }
    //cout << "Time: " << t_total << endl;
    return t_total;
}  

double convert_to_seconds_motor(Robot& robot, string unit, double value, string port) {
    if(unit == "s") return value;
    else if(unit == "rotations"){
        return value * 0.393 / (robot.motor_states[port]->speed / 100) + (min(0.4, 0.4 * value)) * (robot.motor_states[port]->speed / 100) * (robot.motor_states[port]->speed / 100);
    }
    else if(unit == "degrees"){
        value = value/360;
        return value * 0.393 / (robot.motor_states[port]->speed / 100) + (min(0.4, 0.4 * value)) * (robot.motor_states[port]->speed / 100) * (robot.motor_states[port]->speed / 100);
    }
    else value = 0;
    //cout << "Value: " << value << endl;
    double v_max = (robot.motor_states[port]->current_speed / 100) / 0.387;
    double s_acc = v_max * v_max / (2 * robot.motion_vector.acceleration);

    double t_total = 0;
    if (value > 2 * s_acc) {
        // Trapezni profil (ima dio s max brzinom)
        t_total = 2 * (v_max / robot.motion_vector.acceleration) + (value - 2 * s_acc) / v_max;
    } else {
        // Trokutasti profil (nikad ne dosegne max brzinu)
        t_total = 2 * sqrt(value / robot.motion_vector.acceleration);
    }
    //cout << "Time: " << t_total << endl;
    return t_total;
}  

bool is_number(const std::string& s) {
    try {
        std::size_t pos;
        std::stod(s, &pos);
        return pos == s.size();
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

bool is_integer(const std::string& s) {
    try {
        std::size_t pos;
        std::stoi(s, &pos);
        return pos == s.size();
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}
bool calculate_pressed_event(ForceSensor* state, string event){
    if(state->value >= 3 && state->previous_value < state->value && event == "pressed"){
        return true;
    } else if(state->value == 0 && state->previous_value > 0 && event == "released"){
        return true;
    } else if (state->value >= 5 && state->previous_value < state->value && event == "hard-pressed"){
        return true;
    } else if(state->value != state->previous_value && event == "pressure changed"){
        return true;
    } else {
        return false;
    }
}

bool check_distance(double distance, string option, double value){ //TODO: check what tf % is and how it translates into cm
    if(option == "closer than"){
        return distance < value;
    } else if(option == "farther than"){
        return distance > value;
    } else if(option == "exactly at"){
        return distance == value;
    }else {
        return false;
    }
}

void rotate_matrix_left(Robot& robot){
    double temp[5][5];
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            temp[j][i] = robot.pixel_display[i][j];
        }
    }

    // Preokreni redoslijed stupaca
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            robot.pixel_display[i][j] = temp[i][4 - j];
        }
    }


    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            temp[j][i] = robot.permanent_pixel_display[i][j];
        }
    }

    // Preokreni redoslijed stupaca
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            robot.permanent_pixel_display[i][j] = temp[i][4 - j];
        }
    }

    robot.absolute_image_position = (robot.absolute_image_position + 3) % 4;
}

void rotate_matrix_right(Robot& robot) {
    double temp[5][5];

    // Transponiraj matricu
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            temp[j][i] = robot.pixel_display[i][j];
        }
    }

    // Preokreni redoslijed redaka
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            robot.pixel_display[i][j] = temp[4 - i][j];
        }
    }


    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            temp[j][i] = robot.permanent_pixel_display[i][j];
        }
    }

    // Preokreni redoslijed redaka
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            robot.permanent_pixel_display[i][j] = temp[4 - i][j];
        }
    }
    
    robot.absolute_image_position = (robot.absolute_image_position + 1) % 4;
}

string parse_port(Robot& robot, string port, string type){
    string result = "";
    string all_ports = "ABCDEF";
    if(is_number(port)) return result;
    for (int i = 0; i < all_ports.length(); ++i) {
        if(port.find(string(1, all_ports[i])) != string::npos){
            if(type == "Motor"){
                if(robot.motor_states.find(string(1, all_ports[i])) != robot.motor_states.end()){
                    result += string(1, all_ports[i]);
                }
            } else if(type == "Color"){
                if(robot.color_states.find(string(1, all_ports[i])) != robot.color_states.end()){
                    result += string(1, all_ports[i]);
                }
            } else if(type == "Distance"){
                if(robot.distance_states.find(string(1, all_ports[i])) != robot.distance_states.end()){
                    result += string(1, all_ports[i]);
                }
            } else if(type == "Force"){
                if(robot.force_states.find(string(1, all_ports[i])) != robot.force_states.end()){
                    result += string(1, all_ports[i]);
                }
            }
        }
    }
    return result;
}

bool calculate_direction(Robot& robot, string port, string direction, double position){
    if(direction == "clockwise") return true;
    else if(direction == "counterclockwise") return false;
    else{
        if(static_cast<int>(position - robot.motor_states[port]->value + 360) % 360 > 180){
            return true;
        } else {
            return false;
        }	
    }
}


#endif // FUNCTIONS_H