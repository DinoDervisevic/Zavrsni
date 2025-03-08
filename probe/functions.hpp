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
}  

#endif // FUNCTIONS_H