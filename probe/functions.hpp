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
int convert_to_seconds(Robot& robot, string unit, double value) {
    if(unit == "s") return value;
    double speed = (robot.v1 + robot.v2)/2;
    if(unit == "cm"){
        return value/speed;
    }
    if(unit == "in"){
        return value*2.54/speed;
    }
    if(unit == "rotations"){ // check what happens if we use the "set 1 motor rotation to" block
        return value*robot.wheel_distance/(2*robot.wheel_radius*3.14159*speed);
    }
    if(unit == "degrees"){ // TODO: check if this is correct
        return value/360*robot.wheel_distance/(2*robot.wheel_radius*3.14159*speed);
    }
    return 0;
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

void rotate_matrix_right(Robot& robot){
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

    robot.absolute_image_position = (robot.absolute_image_position + 1) % 4;
}

void rotate_matrix_left(Robot& robot) {
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
    robot.absolute_image_position = (robot.absolute_image_position + 3) % 4;
}

string parse_port(Robot& robot, string port, string type){
    string result = "";
    string all_ports = "ABCDEF";
    if(is_number(port)) return result;
    for (int i = 0; i < all_ports.length(); ++i) {
        if(port.find(string(1, all_ports[i])) != string::npos){
            if(type == "motor"){
                if(robot.motor_states.find(string(1, all_ports[i])) != robot.motor_states.end()){
                    result += string(1, all_ports[i]);
                }
            } else if(type == "color"){
                if(robot.color_states.find(string(1, all_ports[i])) != robot.color_states.end()){
                    result += string(1, all_ports[i]);
                }
            } else if(type == "distance"){
                if(robot.distance_states.find(string(1, all_ports[i])) != robot.distance_states.end()){
                    result += string(1, all_ports[i]);
                }
            } else if(type == "force"){
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