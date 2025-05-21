#ifndef VECTORS_H
#define VECTORS_H

#include <cmath>

#include "robot.hpp"


using namespace std;

void calculate_motor_speed(Robot& robot){
    for(auto i : "ABCDEF"){
        string port(1, i);
        if(robot.motor_states.find(port) != robot.motor_states.end()
            && robot.motor_states[port]->time_left > 0){

            MotorState* motor = robot.motor_states[port];
            double acc = robot.motion_vector.acceleration * 100 * 0.387;
            double stop_time = abs(motor->current_speed) / acc;
            

            if(motor->time_left <= stop_time + 1e-9){
                if(motor->current_speed > 0){
                    motor->current_speed -= acc * robot.discrete_time_interval;
                    if(motor->current_speed < 0) motor->current_speed = 0;
                } 
                else if(motor->current_speed < 0){
                    motor->current_speed += acc * robot.discrete_time_interval;
                    if(motor->current_speed > 0) motor->current_speed = 0;
                }
            }

            else if(abs(motor->current_speed) < abs(motor->value)){
                if(motor->value > 0){
                    motor->current_speed += acc * robot.discrete_time_interval;
                    if(motor->current_speed > motor->value) motor->current_speed = motor->value;
                }
                else if(motor->value < 0){
                    motor->current_speed -= acc * robot.discrete_time_interval;
                    if(motor->current_speed < motor->value) motor->current_speed = motor->value;
                }
            } 
            else{
                motor->current_speed = motor->value;
            }
            
        }
    }
}

void subtract_time_from_motors(Robot& robot){
    for(auto i : "ABCDEF"){
        string port(1, i);
        if(robot.motor_states.find(port) != robot.motor_states.end()){
            robot.motor_states[port]->time_left -= robot.discrete_time_interval;
            if(robot.motor_states[port]->time_left <= 0){
                robot.motor_states[port]->current_speed = 0;
                robot.motor_states[port]->time_left = 0;
            }
        }
    }
}

// Functions to calculate motion vectors for each ribot individually (just this one for now)
void one_motor_two_wheel_robot(Robot& robot){
    double right_wheel_speed = -robot.calculate_wheel_speed("A"); // hardcoded; the right wheel has to be here
    double left_wheel_speed = right_wheel_speed; // hardcoded; the left wheel has to be here
    
    double target_speed = (right_wheel_speed + left_wheel_speed) / 2;
    double target_angular_speed = (right_wheel_speed - left_wheel_speed) / robot.wheel_distance;

    robot.motion_vector.linear_velocity = target_speed;
    robot.motion_vector.angular_velocity = target_angular_speed;
    /*if(robot.motion_vector.linear_velocity < target_speed){
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
    }*/

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
    calculate_motor_speed(robot);
    one_motor_two_wheel_robot(robot);
    calculate_position(robot);
    subtract_time_from_motors(robot);

    for(auto i : "ABCDEF"){
        robot.calculate_motor_position(string(1, i));
    }
}

#endif