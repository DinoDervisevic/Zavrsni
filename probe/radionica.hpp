#ifndef RADIONICA_HPP
#define RADIONICA_HPP
#include <iostream>
#include <string>
#include <map>
#include <math.h>

#include "robot.hpp"
#include "color_names.hpp"

using namespace std;

#define DELTA_T 2.0 // how long the robot must stay still before we conclude the task is over
#define MARGIN_OF_ERROR_LOWER 0.95
#define MARGIN_OF_ERROR_UPPER 1.05
#define EPSILON 0.01

#define ROZA_SPREMNIK 20.0
#define PLAVI_SPREMNIK 40.0
#define CRNI_SPREMNIK 60.0
#define CRVENI_SPREMNIK 80.0
#define ZUTI_SPREMNIK 100.0

#define KOSARICA_OTVARANJE 15
#define KOSARICA_MAX 50

bool check_task_0(Robot& robot){
    return true;
}

bool check_task_1(Robot& robot){
    bool reached_target = false;
    for(int i = 0; i < robot.robot_states.size(); i++){
        if(robot.robot_states[i].x >= 17.43*MARGIN_OF_ERROR_LOWER && robot.robot_states[i].x <= 17.43*MARGIN_OF_ERROR_UPPER){
            reached_target = true;
        }
        else if(reached_target) return false;

    }
    return reached_target;
}

bool check_task_2(Robot& robot){
    bool reached_target = false;
    for(int i = 0; i < robot.robot_states.size(); i++){
        if(robot.robot_states[i].x <= -17.43*MARGIN_OF_ERROR_LOWER && robot.robot_states[i].x >= -17.43*MARGIN_OF_ERROR_UPPER){
            reached_target = true;
        }
        else if(reached_target) return false;

    }
    return reached_target;
}

bool check_task_3(Robot& robot){
    // Pronađi prvi indeks nakon pritiska tipke
    int start_idx = -1;
    for (int i = 0; i < robot.robot_states.size(); ++i) {
        if(fabs(robot.robot_states[i].x) >= EPSILON){
            break;
        }
        if (robot.robot_states[i].t >= 2.0) {
            start_idx = i;
            break;
        }
    }
    if (start_idx == -1) return false;

    // 1. Pronađi trenutak kad je robot stigao do roza spremnika
    int idx_reach = -1;
    for (int i = start_idx; i < robot.robot_states.size(); ++i) {
        double x = robot.robot_states[i].x;
        if (x >= ROZA_SPREMNIK * MARGIN_OF_ERROR_LOWER && x <= ROZA_SPREMNIK * MARGIN_OF_ERROR_UPPER) {
            idx_reach = i;
            break;
        }
    }
    if (idx_reach == -1) return false;

    // 2. Pronađi trenutak kad se vratio u bazu (x ≈ 0)
    int idx_return = -1;
    for (int i = idx_reach; i < robot.robot_states.size(); ++i) {
        double x = robot.robot_states[i].x;
        if (fabs(x) < EPSILON) {
            idx_return = i;
            break;
        }
    }
    if (idx_return == -1) return false;

    // 3. Provjeri da je ostao u bazi
    for (int i = idx_return; i < robot.robot_states.size(); ++i) {
        if (fabs(robot.robot_states[i].x) > EPSILON) {
            return false;
        }
    }

    return true;
}

void outside_interference_task_3(Robot& robot){
    if(robot.time_since_start >= 2.0){
        robot.force_states["C"]->value = 7;
    }
    if(robot.time_since_start >= 2.1){
        robot.force_states["C"]->value = 0;
    }
}

bool check_task_4(Robot& robot){
    bool opened = false;
    for (const auto& state : robot.robot_states) {
        // Pretpostavljamo da je motor košarice na portu "C"
        double pos = state.motor_states.at("B");
        if (!opened && pos >= KOSARICA_OTVARANJE) {
            opened = true;
        }
        // Nakon što je otvorena, traži zatvaranje
        if (opened && fabs(pos) < EPSILON) {
            return true;
        }
    }
    return false;
}

bool check_task_5(Robot& robot){
    return true;
}

bool check_task_6(Robot& robot){
    bool reached_spremnik = false;
    bool opened = false;
    bool closed = false;

    // 1. Pronađi dolazak do plavog spremnika
    for (int i = 0; i < robot.robot_states.size(); ++i) {
        double x = robot.robot_states[i].x;
        if (!reached_spremnik && x >= PLAVI_SPREMNIK * MARGIN_OF_ERROR_LOWER && x <= PLAVI_SPREMNIK * MARGIN_OF_ERROR_UPPER) {
            reached_spremnik = true;
        }
        // 2. Otvaranje košarice nakon dolaska
        if (reached_spremnik && !opened && robot.robot_states[i].motor_states.at("B") >= KOSARICA_OTVARANJE) {
            opened = true;
        }
        // 3. Zatvaranje košarice nakon otvaranja
        if (opened && !closed && fabs(robot.robot_states[i].motor_states.at("B")) < EPSILON) {
            closed = true;
        }
        // 4. Povratak u bazu nakon zatvaranja
        if (closed && fabs(x) < EPSILON) {
            return true;
        }
    }
    return false;
}

bool check_task_7(Robot& robot){
    bool reached_spremnik = false;
    bool opened = false;
    bool closed = false;

    // 1. Pronađi dolazak do plavog spremnika
    for (int i = 0; i < robot.robot_states.size(); ++i) {
        double x = robot.robot_states[i].x;
        if (!reached_spremnik && x >= PLAVI_SPREMNIK * MARGIN_OF_ERROR_LOWER && x <= PLAVI_SPREMNIK * MARGIN_OF_ERROR_UPPER) {
            reached_spremnik = true;
        }
        // 2. Otvaranje košarice nakon dolaska
        if (reached_spremnik && !opened && robot.robot_states[i].motor_states.at("B") >= KOSARICA_OTVARANJE) {
            opened = true;
        }
        // 3. Zatvaranje košarice nakon otvaranja
        if (opened && !closed && fabs(robot.robot_states[i].motor_states.at("B")) < EPSILON) {
            closed = true;
        }
        // 4. Povratak u bazu nakon zatvaranja
        if (closed && fabs(x) < EPSILON) {
            return true;
        }
    }
    return false;
}

bool check_task_8(Robot& robot){
    return true;
}

bool check_task_9(Robot& robot){
    return true;
}

bool check_task_10(Robot& robot){
    return true;
}

bool check_task_11(Robot& robot){
    bool reached_spremnik = false;
    bool opened = false;
    bool closed = false;

    // Pronađi prvi indeks nakon pritiska tipke
    int start_idx = -1;
    for (int i = 0; i < robot.robot_states.size(); ++i) {
        if(fabs(robot.robot_states[i].x) >= EPSILON){
            break;
        }
        if (robot.robot_states[i].t >= 2.0) {
            start_idx = i;
            break;
        }
    }
    if (start_idx == -1) return false;

    // 1. Pronađi dolazak do plavog spremnika
    for (int i = start_idx; i < robot.robot_states.size(); ++i) {
        double x = robot.robot_states[i].x;
        if (!reached_spremnik && x >= PLAVI_SPREMNIK * MARGIN_OF_ERROR_LOWER && x <= PLAVI_SPREMNIK * MARGIN_OF_ERROR_UPPER) {
            reached_spremnik = true;
        }
        // 2. Otvaranje košarice nakon dolaska
        if (reached_spremnik && !opened && robot.robot_states[i].motor_states.at("B") >= KOSARICA_OTVARANJE) {
            opened = true;
        }
        // 3. Zatvaranje košarice nakon otvaranja
        if (opened && !closed && fabs(robot.robot_states[i].motor_states.at("B")) < EPSILON) {
            closed = true;
        }
        // 4. Povratak u bazu nakon zatvaranja
        if (closed && fabs(x) < EPSILON) {
            return true;
        }
    }
    return false;
}

void outside_interference_task_11(Robot& robot){
    if(robot.time_since_start >= 2.0){
        robot.force_states["D"]->value = color_names.at("blue");
    }
    if(robot.time_since_start >= 2.1){
        robot.force_states["D"]->value = color_names.at("none");
    }
}

bool check_task_12(Robot& robot){
    bool reached_spremnik = false;
    bool opened = false;
    bool closed = false;

    // Pronađi prvi indeks nakon pritiska tipke
    int start_idx = -1;
    for (int i = 0; i < robot.robot_states.size(); ++i) {
        if(fabs(robot.robot_states[i].x) >= EPSILON){
            break;
        }
        if (robot.robot_states[i].t >= 2.0) {
            start_idx = i;
            break;
        }
    }
    if (start_idx == -1) return false;

    // 1. Pronađi dolazak do plavog spremnika
    for (int i = start_idx; i < robot.robot_states.size(); ++i) {
        double x = robot.robot_states[i].x;
        if (!reached_spremnik && x >= CRNI_SPREMNIK * MARGIN_OF_ERROR_LOWER && x <= CRNI_SPREMNIK * MARGIN_OF_ERROR_UPPER) {
            reached_spremnik = true;
        }
        // 2. Otvaranje košarice nakon dolaska
        if (reached_spremnik && !opened && robot.robot_states[i].motor_states.at("B") >= KOSARICA_OTVARANJE) {
            opened = true;
        }
        // 3. Zatvaranje košarice nakon otvaranja
        if (opened && !closed && fabs(robot.robot_states[i].motor_states.at("B")) < EPSILON) {
            closed = true;
        }
        // 4. Povratak u bazu nakon zatvaranja
        if (closed && fabs(x) < EPSILON) {
            return true;
        }
    }
    return false;
}

void outside_interference_task_12(Robot& robot){
    if(robot.time_since_start >= 2.0){
        robot.force_states["D"]->value = color_names.at("black");
    }
    if(robot.time_since_start >= 2.1){
        robot.force_states["D"]->value = color_names.at("none");
    }
}

bool check_task_13(Robot& robot){
    double time = 0;
    int starting_index = 0;
    int cn = 0;
    vector<double> spremnici = {CRVENI_SPREMNIK, PLAVI_SPREMNIK, ZUTI_SPREMNIK};
    for (int k = 0; k < 3; ++k){
        if(cn < k) return false;
        bool reached_spremnik = false;
        bool opened = false;
        bool closed = false;

        // Pronađi prvi indeks nakon sto senzor uhvati boju
        int start_idx = -1;
        for (int i = starting_index; i < robot.robot_states.size(); ++i) {
            if(fabs(robot.robot_states[i].x) >= EPSILON){
                break;
            }
            if (robot.robot_states[i].t >= time + 2.0) {
                start_idx = i;
                break;
            }
        }
        if (start_idx == -1) return false;

        // 1. Pronađi dolazak do spremnika
        for (int i = start_idx; i < robot.robot_states.size(); ++i) {
            double x = robot.robot_states[i].x;
            if (!reached_spremnik && x >= spremnici[k] * MARGIN_OF_ERROR_LOWER && x <= spremnici[k] * MARGIN_OF_ERROR_UPPER) {
                reached_spremnik = true;
            }
            // 2. Otvaranje košarice nakon dolaska
            if (reached_spremnik && !opened && robot.robot_states[i].motor_states.at("B") >= KOSARICA_OTVARANJE) {
                opened = true;
            }
            // 3. Zatvaranje košarice nakon otvaranja
            if (opened && !closed && fabs(robot.robot_states[i].motor_states.at("B")) < EPSILON) {
                closed = true;
            }
            // 4. Povratak u bazu nakon zatvaranja
            if (closed && fabs(x) < EPSILON) {
                cn++;
                time = robot.robot_states[i].t + robot.discrete_time_interval;
                starting_index = i+1;
            }
        }
    }
    if(cn == 3) return true;
    else return false;
}

void outside_interference_task_13(Robot& robot){
    if(robot.time_since_start >= 2.0){
        robot.force_states["D"]->value = color_names.at("red");
    }
    if(robot.time_since_start >= 2.1){
        robot.force_states["D"]->value = color_names.at("none");
    }
    if(robot.time_since_start >= 12.0){
        robot.force_states["D"]->value = color_names.at("blue");
    }
    if(robot.time_since_start >= 12.1){
        robot.force_states["D"]->value = color_names.at("none");
    }
    if(robot.time_since_start >= 22.0){
        robot.force_states["D"]->value = color_names.at("yellow");
    }
    if(robot.time_since_start >= 22.1){
        robot.force_states["D"]->value = color_names.at("none");
    }
}

bool check_task_14(Robot& robot){
    return true;
}

void adjust_trash_can(Robot& robot){
    if(robot.motor_states["B"]->position >= KOSARICA_MAX){
        if(robot.motor_states["B"]->position - KOSARICA_MAX < (360 - KOSARICA_MAX)/2){
            robot.motor_states["B"]->position = KOSARICA_MAX;
        }
        else{
            robot.motor_states["B"]->position = 0;
        }
    }
}

#endif