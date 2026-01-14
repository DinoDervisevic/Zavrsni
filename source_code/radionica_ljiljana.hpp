#ifndef RADIONICA_LJILJANA_HPP
#define RADIONICA_LJILJANA_HPP
#include <iostream>
#include <string>
#include <map>
#include <math.h>

#include "robot/Robot.hpp"
#include "utils/color_names.hpp"

using namespace std;

#define DELTA_T 0.5
#define MARGIN_OF_ERROR 0.03

#define MOVED_A_BIT 10

void outside_interference(Robot& robot){
    if (robot.time_since_start >= 10.0 && robot.time_since_start < 11) {
        robot.buttons["left"] = "pressed";
        robot.buttons["right"] = "pressed";
    } else {
        robot.buttons["left"] = "released";
        robot.buttons["right"] = "released";
    }
}

bool check_if_moved_a_bit(Robot& robot, string motor, int i){
    if((robot.robot_states[i].motor_states.at(motor) > MOVED_A_BIT && robot.robot_states[i].motor_states.at(motor) < 180)
        || (robot.robot_states[i].motor_states.at(motor) < 360 - MOVED_A_BIT && robot.robot_states[i].motor_states.at(motor) > 180)){
        return true;
    }
    return false;
}

int check_task_1(Robot& robot){
    int score = 0;
    bool poceo_kretat = false;
    bool reached_target = false;
    bool halfway = false;

    for(int i = 0; i < robot.robot_states.size(); i++){
        //cout << robot.robot_states[i].motor_states.at("D") << endl;
        if (!poceo_kretat && check_if_moved_a_bit(robot, "D", i)) {
            poceo_kretat = true;
            score += 30;
        }

        if (poceo_kretat && !halfway) {
            float pos = robot.robot_states[i].motor_states.at("D");
            if ((pos > 180 - MARGIN_OF_ERROR * 360 && pos < 180 + MARGIN_OF_ERROR * 360)) {
                halfway = true;
            }
        }

        if (poceo_kretat && !reached_target && halfway) {
            float pos = robot.robot_states[i].motor_states.at("D");
            if ((pos > 360 - MARGIN_OF_ERROR * 360 && pos < 360) ||
                (pos < 0 + MARGIN_OF_ERROR * 360 && pos > 0)) {
                reached_target = true;
                score += 70;
            }
        }

        if (reached_target) {
            float pos = robot.robot_states[i].motor_states.at("D");
            bool in_range = (pos > 360 - MARGIN_OF_ERROR * 360 && pos <= 360) ||
                            (pos < 0 + MARGIN_OF_ERROR * 360 && pos >= 0);
            if (!in_range) {
                score -= 50;
                break;
            }
        }
    }

    return score;
}

int check_task_2(Robot& robot){
    int score = 0;
    bool poceo_kretat_D = false;
    bool poceo_kretat_F = false;
    bool reached_target = false;

    for(int i = 0; i < robot.robot_states.size(); i++){
        if ((!poceo_kretat_D && abs(robot.robot_states[i].motor_states.at("D")) > MOVED_A_BIT)
            && (!poceo_kretat_F && abs(robot.robot_states[i].motor_states.at("F")) > MOVED_A_BIT)) {
            poceo_kretat_D = true;
            poceo_kretat_F = true;
            score += 100;
        }
    }

    return score;
}

int check_task_3(Robot& robot){
    int score = 0;
    bool poceo_kretat_D = false;
    bool poceo_kretat_F = false;

    bool D_moving_clockwise = false;
    bool D_reached_target = false;
    bool D_done = false;
    bool D_correct_speed = false;
    bool D_done_halfway = false;

    float F_started_moving_time = 0.0;
    bool F_done = false;
    bool F_correct_speed = false;

    for(int i = 0; i < robot.robot_states.size(); i++){
        if (!poceo_kretat_D && !D_done && check_if_moved_a_bit(robot, "D", i)) {
            poceo_kretat_D = true;
            score += 10;
            if (robot.robot_states[i].motor_states.at("D") > 180) D_moving_clockwise = true;
            else D_moving_clockwise = false;

            if(robot.robot_states[i].motor_states_speed.at("D") == 30){
                D_correct_speed = true;
            }
        }

        if (poceo_kretat_D){
            if (!D_done_halfway){
                if (robot.robot_states[i].motor_states.at("D") > 180 - MARGIN_OF_ERROR * 360 && robot.robot_states[i].motor_states.at("D") < 180 + MARGIN_OF_ERROR * 360){
                    D_done_halfway = true;
                }
            }
        }

        if(poceo_kretat_D && D_done_halfway && !D_done && !D_reached_target){
            if (D_moving_clockwise && (robot.robot_states[i].motor_states.at("D") > 15 - MARGIN_OF_ERROR * 360 && robot.robot_states[i].motor_states.at("D") < 15 + MARGIN_OF_ERROR * 360)){
                D_reached_target = true;
                score += 20;
            }
            if (!D_moving_clockwise && (robot.robot_states[i].motor_states.at("D") > 345 - MARGIN_OF_ERROR * 360 && robot.robot_states[i].motor_states.at("D") < 345 + MARGIN_OF_ERROR * 360)){
                D_reached_target = true;
                score += 20;
            }
        }

        if (D_reached_target && !D_done){
            if (D_moving_clockwise && !(robot.robot_states[i].motor_states.at("D") > 15 - MARGIN_OF_ERROR * 360 && robot.robot_states[i].motor_states.at("D") < 15 + MARGIN_OF_ERROR * 360)){
                D_done = true;
                score -= 20;
            }
            if (!D_moving_clockwise && !(robot.robot_states[i].motor_states.at("D") > 345 - MARGIN_OF_ERROR * 360 && robot.robot_states[i].motor_states.at("D") < 345 + MARGIN_OF_ERROR * 360)){
                D_done = true;
                score -= 20;
            }
        }


        if (!poceo_kretat_F && check_if_moved_a_bit(robot, "F", i)) {
            poceo_kretat_F = true;
            score += 10;
            F_started_moving_time = robot.robot_states[i].t;

            if(robot.robot_states[i].motor_states_speed.at("F") == 50){
                F_correct_speed = true;
            }
        }

        if (poceo_kretat_F && robot.robot_states[i].t - F_started_moving_time < 5.0){
            if(robot.robot_states[i].motor_states_value.at("F") == 0){
                F_done = true;
            }
        }
        if(!F_done && robot.robot_states[i].t - F_started_moving_time > 5.5){
            if(robot.robot_states[i].motor_states_value.at("F") == 0){
                score += 20;
                F_done = true;
            }
        }

        if(F_done && D_reached_target){
            if(D_correct_speed && F_correct_speed){
                score += 40;
            }
            else{
                if(robot.robot_states[i].motor_states_speed.at("F") == 50) score += 10;
                if(robot.robot_states[i].motor_states_speed.at("D") == 30) score += 10;
                if(robot.robot_states[i].motor_states_speed.at("D") == 30 && robot.robot_states[i].motor_states_speed.at("F") == 50) score += 10;
            }
            break;
        }
    }

    return score;
}


int check_task_4(Robot& robot){
    int score = 0;
    bool poceo_kretat_D = false;
    bool poceo_kretat_F = false;

    int D_position_one = -1;
    int D_position_two = -1;
    bool D_moving_clockwise = false;
    bool D_done = false;
    bool D_correct_speed = false;
    bool D_done_halfway = false;
    float D_started_moving_time = 0.0;

    bool F_done = false;
    bool F_correct_speed = false;
    bool F_moving_clockwise = false;
    bool F_done_halfway = false;
    float F_started_moving_time = 0.0;


    for(int i = 0; i < robot.robot_states.size(); i++){

        if (!poceo_kretat_D && !D_done && check_if_moved_a_bit(robot, "D", i)) {
            poceo_kretat_D = true;
            D_started_moving_time = robot.robot_states[i].t;
            score += 5;
            if (robot.robot_states[i].motor_states.at("D") > 180) D_moving_clockwise = true;
            else D_moving_clockwise = false;

            if(robot.robot_states[i].motor_states_speed.at("D") == 20){
                D_correct_speed = true;
            }
        }

        if (poceo_kretat_D){
            if (!D_done_halfway){
                if (robot.robot_states[i].motor_states.at("D") > 180 - MARGIN_OF_ERROR * 360 && robot.robot_states[i].motor_states.at("D") < 180 + MARGIN_OF_ERROR * 360){
                    D_done_halfway = true;
                }
            }
        }

        if (poceo_kretat_D && !D_done && D_position_one == -1 && D_done_halfway){
            if (robot.robot_states[i].motor_states.at("D") > 360 - MARGIN_OF_ERROR * 360 || robot.robot_states[i].motor_states.at("D") < 0 + MARGIN_OF_ERROR * 360){
                D_position_one = i;
                D_done_halfway = false;
            }
        }
        else if(poceo_kretat_D && !D_done && D_position_one != -1 && D_position_two == -1 && D_done_halfway){
            if (robot.robot_states[i].motor_states.at("D") > 360 - MARGIN_OF_ERROR * 360 || robot.robot_states[i].motor_states.at("D") < 0 + MARGIN_OF_ERROR * 360){
                D_position_two = i;
                D_done_halfway = false;
            }
        }
        else if(poceo_kretat_D && !D_done && D_position_one != -1 && D_position_two != -1 && D_done_halfway){
            if (robot.robot_states[i].motor_states.at("D") > 360 - MARGIN_OF_ERROR * 360 || robot.robot_states[i].motor_states.at("D") < 0 + MARGIN_OF_ERROR * 360){
                score += 10;
                D_done = true;
            }
        }


        if (!poceo_kretat_F && check_if_moved_a_bit(robot, "F", i)) {
            poceo_kretat_F = true;
            F_started_moving_time = robot.robot_states[i].t;
            score += 5;
            if (robot.robot_states[i].motor_states.at("F") > 180) F_moving_clockwise = true;
            else F_moving_clockwise = false;

            if(robot.robot_states[i].motor_states_speed.at("F") == 100){
                F_correct_speed = true;
            }
        }

        if (poceo_kretat_F){
            if (!F_done_halfway){
                if (robot.robot_states[i].motor_states.at("F") > 180 - MARGIN_OF_ERROR * 360 && robot.robot_states[i].motor_states.at("F") < 180 + MARGIN_OF_ERROR * 360){
                    F_done_halfway = true;
                }
            }
        }

        if (poceo_kretat_F && F_done_halfway && !F_done){
            if(!F_moving_clockwise){
                F_done = true;
            }
            else{
                if (robot.robot_states[i].motor_states.at("F") < 0 + MARGIN_OF_ERROR * 360 || robot.robot_states[i].motor_states.at("F") > 360 - MARGIN_OF_ERROR * 360){
                    F_done = true;
                    score += 10;
                }
            }
        }   

        if(F_done && D_done){
            if(D_correct_speed && F_correct_speed){
                score += 20;
            }
            else{
                if(robot.robot_states[i].motor_states_speed.at("F") == 100) score += 5;
                if(robot.robot_states[i].motor_states_speed.at("D") == 20) score += 5;
            }
            if (D_started_moving_time - 0.5 < F_started_moving_time && F_started_moving_time < D_started_moving_time + 0.5){
                score += 50;
            }
            break;
        }
        
    }

    return score;
}

void outside_interference_5(Robot& robot){
    if (!robot.distance_states.count("B") || !robot.distance_states["B"]) return;
    if (robot.time_since_start < 4.1) {
        robot.distance_states["B"]->value = 100.0; // daleko
    } else {
        robot.distance_states["B"]->value = 4.0;   // bliže od 5cm
    }
}

int check_task_5(Robot& robot){
    int score = 50;

    bool poceo_kretat_D = false;
    bool poceo_kretat_F = false;

    bool D_moving_clockwise = false;
    bool D_done = false;
    bool D_correct_speed = false;
    float D_started_moving_time = 0.0;

    bool F_done = false;
    bool F_correct_speed = false;
    bool F_moving_clockwise = false;
    float F_started_moving_time = 0.0;

    for (int i = 0; i < robot.robot_states.size(); i++){
        if (!poceo_kretat_D && !D_done && check_if_moved_a_bit(robot, "D", i)) {
            poceo_kretat_D = true;
            D_started_moving_time = robot.robot_states[i].t;
            score += 5;
            if (robot.robot_states[i].motor_states.at("D") > 180) D_moving_clockwise = true;
            else D_moving_clockwise = false;

            if(robot.robot_states[i].motor_states_speed.at("D") == 50){
                D_correct_speed = true;
            }
        }

        if (!poceo_kretat_F && check_if_moved_a_bit(robot, "F", i)) {
            poceo_kretat_F = true;
            F_started_moving_time = robot.robot_states[i].t;
            score += 5;
            if (robot.robot_states[i].motor_states.at("F") > 180) F_moving_clockwise = true;
            else F_moving_clockwise = false;

            if(robot.robot_states[i].motor_states_speed.at("F") == 50){
                F_correct_speed = true;
            }
        }

        if (poceo_kretat_D && !D_done){
            if (robot.robot_states[i].distance_states.at("B") > 5.0){
                if (robot.robot_states[i].motor_states_value.at("D") == 0){
                    D_done = true;
                    score -= 25;
                }
            }
            else{
                if (robot.robot_states[i].motor_states_value.at("D") != 0){
                    D_done = true;
                    score -= 25;
                }
            }
        }
        // problem tu je sto ovo kad je tipa jedan rotation u while true petlji zavrsen, onda ce se na onak 2 framea ugasit kretanje, 
        //a onda odmah oduzimam bodove i gasim pa trebam napraviti da ne pratim value tu neg 
        //da idem tipa gledat za svakih pola sekunde jel se pomaknuo motor za neku min vrijednost
        if (poceo_kretat_F && !F_done){
            if (robot.robot_states[i].distance_states.at("B") > 5.0){
                if (robot.robot_states[i].motor_states_value.at("F") == 0){
                    F_done = true;
                    score -= 25;
                }
            }
            else{
                if (robot.robot_states[i].motor_states_value.at("F") != 0){
                    F_done = true;
                    score -= 25;
                }
            }
        }
        if (i == robot.robot_states.size() - 1){
            D_done = true;
            F_done = true;
        }

        if(F_done && D_done){
            if(D_correct_speed && F_correct_speed){
                score += 20;
            }
            else{
                if(robot.robot_states[i].motor_states_speed.at("F") == 50) score += 5;
                if(robot.robot_states[i].motor_states_speed.at("D") == 50) score += 5;
            }
            if (D_started_moving_time - 0.5 < F_started_moving_time && F_started_moving_time < D_started_moving_time + 0.5){
                score += 20;
            }
            break;
        }
    }

    return score;
}

void outside_interference_6(Robot& robot){
    if (!robot.distance_states.count("B") || !robot.distance_states["B"]) return;

    if (robot.time_since_start < 1.0) {
        robot.distance_states["B"]->value = 100.0;
    } else if (robot.time_since_start < 1.1) {
        robot.distance_states["B"]->value = 4.0;
    } else if (robot.time_since_start < 200.0) {
        robot.distance_states["B"]->value = 100.0;
    } else if (robot.time_since_start < 200.1) {
        robot.distance_states["B"]->value = 4.0;
    } else {
        robot.distance_states["B"]->value = 100.0;
    }
}

int check_task_6(Robot& robot){
    int score = 0;
    bool poceo_kretat = false;

    bool simulatanious_starts = true; // until proven otherwise

    float D_start_time = -1.0;
    float F_start_time = -1.0;
    float start_time = -1.0;

    bool is_moving_correctly = true; // until proven otherwise
    int times_moved = 0;
    int times_stopped = 0;


    for(int i = 0; i < robot.robot_states.size(); i++){
        //cout << "Time: " << robot.robot_states[i].t << " D: " << robot.robot_states[i].motor_states.at("D") << " F: " << robot.robot_states[i].motor_states.at("F") << "B: " << robot.robot_states[i].distance_states.at("B") << endl;
        if (D_start_time < 0.0 && check_if_moved_a_bit(robot, "D", i) && robot.robot_states[i].motor_states_value.at("D") != 0) {
            D_start_time = robot.robot_states[i].t;
            poceo_kretat = true;
            if (start_time < 0.0){
                start_time = D_start_time;
                score += 10;
                times_moved += 1;
            }
            else {
                if (fabs(D_start_time - start_time) > 0.5) simulatanious_starts = false;
            }

            if(!(robot.robot_states[i].t > 0.5 && robot.robot_states[i].t < 1.5) &&
               !(robot.robot_states[i].t > 199.5 && robot.robot_states[i].t < 200.5)) {
                is_moving_correctly = false;
            }
        }
        if (F_start_time < 0.0 && check_if_moved_a_bit(robot, "F", i) && robot.robot_states[i].motor_states_value.at("F") != 0) {
            F_start_time = robot.robot_states[i].t;
            poceo_kretat = true;
            if (start_time < 0.0){
                start_time = F_start_time;
                score += 10;
                times_moved += 1;
            }
            else {
                if (fabs(F_start_time - start_time) > 0.5) simulatanious_starts = false;
            }
            if(!(robot.robot_states[i].t > 0.5 && robot.robot_states[i].t < 1.5) &&
               !(robot.robot_states[i].t > 199.5 && robot.robot_states[i].t < 200.5)) {
                is_moving_correctly = false;
            }
        }

        if (poceo_kretat){
            if (robot.robot_states[i].motor_states_value.at("D") == 0 && robot.robot_states[i].motor_states_value.at("F") == 0){
                times_stopped += 1;
                poceo_kretat = false;
                //cout << "Both motors stopped at time: " << robot.robot_states[i].t << endl;
                score += 5;

                if (!simulatanious_starts || D_start_time < 0.0 || F_start_time < 0.0) {
                    is_moving_correctly = false;
                }

                if (fabs((robot.robot_states[i].t - start_time) - 180.0) > 0.5) {
                    is_moving_correctly = false;
                }

                D_start_time = -1.0;
                F_start_time = -1.0;
                start_time = -1.0;
                simulatanious_starts = true;
            }
        }
    }

    if (times_moved != 2 || times_stopped != 2) {
        score -= max(0, 10 * (times_moved - 1));
        score -= max(0, 5 * (times_stopped - 2));
        if (is_moving_correctly && (times_moved != 1 || times_stopped != 1)) {
            is_moving_correctly = false;
        }
    }

    if (is_moving_correctly) {
        score += 70;
    }

    return score;
}

#endif