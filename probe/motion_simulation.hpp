#ifndef VECTORS_H
#define VECTORS_H

#include <cmath>

#include "robot.hpp"


using namespace std;


// Functions to calculate motion vectors for each ribot individually (just this one for now)
void one_motor_two_wheel_robot(Robot& robot){
    double right_wheel_speed = -robot.calculate_wheel_speed("A"); // hardcoded; the right wheel has to be here
    double left_wheel_speed = right_wheel_speed; // hardcoded; the left wheel has to be here
    
    double target_speed = (right_wheel_speed + left_wheel_speed) / 2;
    double target_angular_speed = (right_wheel_speed - left_wheel_speed) / robot.wheel_distance;

    if(robot.motion_vector.linear_velocity < target_speed){
        robot.motion_vector.linear_velocity += robot.motion_vector.acceleration * robot.discrete_time_interval;
        robot.motion_vector.linear_velocity = min(robot.motion_vector.linear_velocity, target_speed);
    } 
    else if(robot.motion_vector.linear_velocity > target_speed){
        robot.motion_vector.linear_velocity = target_speed;
    }
    
    if(robot.motion_vector.angular_velocity > target_angular_speed){
        robot.motion_vector.angular_velocity -= robot.motion_vector.acceleration * robot.discrete_time_interval;
        robot.motion_vector.angular_velocity = max(robot.motion_vector.angular_velocity, target_angular_speed);
    } 
    else if(robot.motion_vector.angular_velocity < target_angular_speed){
        robot.motion_vector.angular_velocity = target_angular_speed;
    }

    //cout << "angular velocity: " << robot.motion_vector.angular_velocity << endl;
}


// Function to calculate position of the differential drive robot
void calculate_position(Robot& robot){
    //cout << robot.motion_vector.linear_velocity << " " << robot.discrete_time_interval << endl;
    double delta_x = robot.motion_vector.linear_velocity * cos(robot.angle * 3.14159265358979323846 / 180) * robot.discrete_time_interval;
    double delta_y = robot.motion_vector.linear_velocity * sin(robot.angle * 3.14159265358979323846 / 180) * robot.discrete_time_interval;
    robot.x += delta_x;
    robot.y += delta_y;
    robot.angle += robot.motion_vector.angular_velocity * robot.discrete_time_interval;
    if (robot.angle >= 360) {
        robot.angle -= 360;
    } else if (robot.angle < 0) {
        robot.angle += 360;
    }
}



// Funal function to calculate everything using these above
void run_robot(Robot& robot){
    one_motor_two_wheel_robot(robot);
    calculate_position(robot);

    for(auto i : "ABCDEF"){
        robot.calculate_motor_position(string(1, i));
    }
}

#endif