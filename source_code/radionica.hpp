#ifndef RADIONICA_HPP
#define RADIONICA_HPP
#include <iostream>
#include <string>
#include <map>
#include <math.h>
#include <vector>

#include "robot/Robot.hpp"
#include "utils/color_names.hpp"

using namespace std;

#define DELTA_T 2.0 // how long the robot must stay still before we conclude the task is over
#define MARGIN_OF_ERROR_LOWER 0.95
#define MARGIN_OF_ERROR_UPPER 1.05
#define EPSILON 0.01

#define OKRETAJ 17.436
#define ROZA_SPREMNIK 17.436*2
#define PLAVI_SPREMNIK 17.436*3
#define CRNI_SPREMNIK 17.436*3
#define CRVENI_SPREMNIK 17.436*4
#define ZUTI_SPREMNIK 17.436*5

#define KOSARICA_OTVARANJE 15
#define KOSARICA_MAX 50

#define DEFAULT_SPEED 75

#define MOVED_A_BIT 10

struct TaskResult {
    int score;
    vector<bool> criteria_met;

    TaskResult(int score, vector<bool> criteria_met) : score(score), criteria_met(criteria_met) {
    };
};


bool check_if_moved_a_bit(Robot& robot, string motor, int i){
    if((robot.robot_states[i].motor_states.at(motor) > MOVED_A_BIT && robot.robot_states[i].motor_states.at(motor) < 180)
        || (robot.robot_states[i].motor_states.at(motor) < 360 - MOVED_A_BIT && robot.robot_states[i].motor_states.at(motor) > 180)){
        return true;
    }
    return false;
}

TaskResult check_task_0(Robot& robot){
    return TaskResult(0, {true});
}

// Robot mora prijeci jedno okretanje, odnosno 17.436 cm i stati
TaskResult check_task_1(Robot& robot){
    bool reached_target = false;
    bool moved_a_bit = false;
    for(int i = 0; i < robot.robot_states.size(); i++){
        if(check_if_moved_a_bit(robot, "A", i)) {
            moved_a_bit = true;
        }

        if(-robot.robot_states[i].x >= OKRETAJ*MARGIN_OF_ERROR_LOWER && -robot.robot_states[i].x <= OKRETAJ*MARGIN_OF_ERROR_UPPER){
            reached_target = true;
        }
        else if(reached_target) return TaskResult(0, {moved_a_bit, false});

    }
    return TaskResult(0, {moved_a_bit, reached_target});
}

//Robot mora prijeci 1 okretanje unatrag
TaskResult check_task_2(Robot& robot){
    bool reached_target = false;
    bool moved_a_bit = false;
    for(int i = 0; i < robot.robot_states.size(); i++){
        if(check_if_moved_a_bit(robot, "A", i)) {
            moved_a_bit = true;
        }
        if(-robot.robot_states[i].x <= -OKRETAJ*MARGIN_OF_ERROR_LOWER && -robot.robot_states[i].x >= -OKRETAJ*MARGIN_OF_ERROR_UPPER){
            reached_target = true;
        }
        else if(reached_target) return TaskResult(0, {moved_a_bit, false});

    }
    return TaskResult(0, {moved_a_bit, reached_target});
}

//Robot se mora kretati do roza spremnika i nazad
TaskResult check_task_3(Robot& robot){
    // cerr << "[DEBUG] Number of saved states: "
    //  << robot.robot_states.size()
    //  << endl;
    // Pronađi prvi indeks nakon pritiska tipke
    int start_idx = -1;
    bool started_moving_only_after_button_press = false;
    bool moved_a_bit = false;
    bool reached_pink_container = false;
    bool returned_to_base = false;
    bool stayed_in_base_after_return = false;

    for (int i = 0; i < robot.robot_states.size(); ++i) {
        if(check_if_moved_a_bit(robot, "A", i)) {
            if (robot.robot_states[i].t >= 2.0) {
                started_moving_only_after_button_press = true;
            }
            moved_a_bit = true;
            start_idx = i;
            break;
        }
    }
    if (start_idx == robot.robot_states.size() || start_idx == -1) {
        return TaskResult(0, {moved_a_bit, started_moving_only_after_button_press, reached_pink_container, returned_to_base, stayed_in_base_after_return});
    }

    // 1. Pronađi trenutak kad je robot stigao do roza spremnika
    for (int i = start_idx; i < robot.robot_states.size(); ++i) {
        double x = -robot.robot_states[i].x;
        if (x >= ROZA_SPREMNIK * MARGIN_OF_ERROR_LOWER && x <= ROZA_SPREMNIK * MARGIN_OF_ERROR_UPPER) {
            start_idx = i;
            reached_pink_container = true;
            break;
        }
    }
    if (start_idx == robot.robot_states.size() || start_idx == -1) {
        return TaskResult(0, {moved_a_bit, started_moving_only_after_button_press, reached_pink_container, returned_to_base, stayed_in_base_after_return});
    }

    // 2. Pronađi trenutak kad se vratio u bazu (x ≈ 0)
    for (int i = start_idx; i < robot.robot_states.size(); ++i) {
        double x = -robot.robot_states[i].x;
        if (fabs(x) < EPSILON) {
            start_idx = i;
            returned_to_base = true;
            break;
        }
    }
    if (start_idx == robot.robot_states.size() || start_idx == -1) {
        return TaskResult(0, {moved_a_bit, started_moving_only_after_button_press, reached_pink_container, returned_to_base, stayed_in_base_after_return});
    }

    // 3. Provjeri da je ostao u bazi
    for (int i = start_idx; i < robot.robot_states.size(); ++i) {
        if (fabs(robot.robot_states[i].x) > EPSILON) {
            return TaskResult(0, {moved_a_bit, started_moving_only_after_button_press, reached_pink_container, returned_to_base, stayed_in_base_after_return});
        }
    }

    stayed_in_base_after_return = true;
    return TaskResult(0, {moved_a_bit, started_moving_only_after_button_press, reached_pink_container, returned_to_base, stayed_in_base_after_return});
}

void outside_interference_task_3(Robot& robot){
    if(robot.time_since_start >= 2.0){
        robot.force_states["C"]->value = 7;
    }
    if(robot.time_since_start >= 2.1){
        robot.force_states["C"]->value = 0;
    }
}

// Robot treba otvoriti i zatvoriti kosaricu
TaskResult check_task_4(Robot& robot){
    bool opened = false;
    bool closed = false;
    for (const auto& state : robot.robot_states) {
        // Pretpostavljamo da je motor košarice na portu B
        double pos = state.motor_states.at("B");
        if (!opened && pos >= KOSARICA_OTVARANJE) {
            opened = true;
        }
        // Nakon što je otvorena, traži zatvaranje
        if (opened && fabs(pos) < EPSILON) {
            closed = true;
        }
    }
    return TaskResult(0, {opened, closed});
}

TaskResult check_task_5(Robot& robot){
    return TaskResult(0, {true, false});
}

// Robot je morao doci do plavog spremnika, otvoriti i zatvoriti kosaricu te se vratiti
TaskResult check_task_6(Robot& robot){
    bool reached_spremnik = false;
    bool opened = false;
    bool closed = false;
    bool returned_to_base = false;

    // 1. Pronađi dolazak do plavog spremnika
    for (int i = 0; i < robot.robot_states.size(); ++i) {
        double x = -robot.robot_states[i].x;
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
            returned_to_base = true;
        }
    }
    return TaskResult(0, {reached_spremnik, opened, closed, returned_to_base});
}

// Isto kao prije samo mora doci do rozog spremnika
TaskResult check_task_7(Robot& robot){
    bool reached_spremnik = false;
    bool opened = false;
    bool closed = false;
    bool returned_to_base = false;

    // 1. Pronađi dolazak do plavog spremnika
    for (int i = 0; i < robot.robot_states.size(); ++i) {
        double x = -robot.robot_states[i].x;
        if (!reached_spremnik && x >= ROZA_SPREMNIK * MARGIN_OF_ERROR_LOWER && x <= ROZA_SPREMNIK * MARGIN_OF_ERROR_UPPER) {
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
            returned_to_base = true;
        }
    }
    return TaskResult(0, {reached_spremnik, opened, closed, returned_to_base});
}

TaskResult check_task_8(Robot& robot){
    return TaskResult(0, {true});
}

TaskResult check_task_9(Robot& robot){
    return TaskResult(0, {true});
}

TaskResult check_task_10(Robot& robot){
    return TaskResult(0, {true});
}

// Robot mora doci do zutog spremnika nakon sto vidi zuto, ostavit smece tamo i vratit se
TaskResult check_task_11(Robot& robot){
    bool started_moving_only_after_color = false;
    bool reached_spremnik = false;
    bool opened = false;
    bool closed = false;
    bool returned_to_base = false;

    // Pronađi prvi indeks nakon pritiska tipke
    int start_idx = -1;
    for (int i = 0; i < robot.robot_states.size(); ++i) {
        if(fabs(robot.robot_states[i].x) >= EPSILON){
            if (robot.robot_states[i].t >= 2.0) {
                started_moving_only_after_color = true;
            }
            start_idx = i;
            break;
        }
    }
    if (start_idx == robot.robot_states.size() || start_idx == -1) {
        started_moving_only_after_color = false;
        return TaskResult(0, {started_moving_only_after_color, reached_spremnik, opened, closed, returned_to_base});
    }

    // 1. Pronađi dolazak do zutog spremnika
    for (int i = start_idx; i < robot.robot_states.size(); ++i) {
        double x = -robot.robot_states[i].x;
        if (!reached_spremnik && x >= ZUTI_SPREMNIK * MARGIN_OF_ERROR_LOWER && x <= ZUTI_SPREMNIK * MARGIN_OF_ERROR_UPPER) {
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
            returned_to_base = true;
            return TaskResult(0, {started_moving_only_after_color, reached_spremnik, opened, closed, returned_to_base});
        }
    }
    return TaskResult(0, {started_moving_only_after_color, reached_spremnik, opened, closed, returned_to_base});
}

void outside_interference_task_11(Robot& robot){
    if(robot.time_since_start >= 2.0){
        robot.color_states["D"]->value = color_names.at("blue");
    }
    if(robot.time_since_start >= 2.1){
        robot.color_states["D"]->value = color_names.at("none");
    }
}

// Isto kao prije samo plavi spremnik i boja
TaskResult check_task_12(Robot& robot){
    bool started_moving_only_after_color = false;
    bool reached_spremnik = false;
    bool opened = false;
    bool closed = false;
    bool returned_to_base = false;

    // Pronađi prvi indeks nakon pritiska tipke
    int start_idx = -1;
    for (int i = 0; i < robot.robot_states.size(); ++i) {
        if(fabs(robot.robot_states[i].x) >= EPSILON){
            if (robot.robot_states[i].t >= 2.0) {
                started_moving_only_after_color = true;
            }
            start_idx = i;
            break;
        }
    }
    if (start_idx == robot.robot_states.size() || start_idx == -1) {
        started_moving_only_after_color = false;
        return TaskResult(0, {started_moving_only_after_color, reached_spremnik, opened, closed, returned_to_base});
    }

    // 1. Pronađi dolazak do plavog spremnika
    for (int i = start_idx; i < robot.robot_states.size(); ++i) {
        double x = -robot.robot_states[i].x;
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
            returned_to_base = true;
            return TaskResult(0, {started_moving_only_after_color, reached_spremnik, opened, closed, returned_to_base});
        }
    }
    return TaskResult(0, {started_moving_only_after_color, reached_spremnik, opened, closed, returned_to_base});
}

void outside_interference_task_12(Robot& robot){
    if(robot.time_since_start >= 2.0){
        robot.color_states["D"]->value = color_names.at("black");
    }
    if(robot.time_since_start >= 2.1){
        robot.color_states["D"]->value = color_names.at("none");
    }
}

// Robot mora kad vidi zutu, plavu ili crvenu boju doci do odgovarajuceg spremnika i ispustiti teret te se vratiti u bazu
TaskResult check_task_13(Robot& robot){
    double time = 0;
    int starting_index = -1;
    int cn = 0;

    bool started_moving_only_after_color_every_time = true;
    bool did_first_part = false;
    bool did_second_part = false;
    bool did_third_part = false;

    int start_idx = 0;

    vector<double> spremnici = {CRVENI_SPREMNIK, PLAVI_SPREMNIK, ZUTI_SPREMNIK};
    for (int k = 0; k < 3; ++k){
        if (start_idx == robot.robot_states.size()) {
            break;
        }
        bool reached_spremnik = false;
        bool opened = false;
        bool closed = false;

        // Pronađi prvi indeks nakon sto senzor uhvati boju
        for (int i = start_idx; i < robot.robot_states.size(); ++i) {
            start_idx = i + 1;
            if(fabs(robot.robot_states[i].x) >= EPSILON){
                if (robot.robot_states[i].t < time + 2.0) {
                    started_moving_only_after_color_every_time = false;
                }
                break;
            }
        }

        // 1. Pronađi dolazak do spremnika
        for (int i = start_idx; i < robot.robot_states.size(); ++i) {
            start_idx = i + 1;
            double x = -robot.robot_states[i].x;
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
                break;
            }
        }
    }
    
    if (cn >= 1) did_first_part = true;
    if (cn >= 2) did_second_part = true;
    if (cn == 3) did_third_part = true;
    return TaskResult(0, {started_moving_only_after_color_every_time, did_first_part, did_second_part, did_third_part});
}

void outside_interference_task_13(Robot& robot){
    if(robot.time_since_start >= 2.0){
        robot.color_states["D"]->value = color_names.at("red");
    }
    if(robot.time_since_start >= 2.1){
        robot.color_states["D"]->value = color_names.at("none");
    }
    if(robot.time_since_start >= 22.0){
        robot.color_states["D"]->value = color_names.at("blue");
    }
    if(robot.time_since_start >= 22.1){
        robot.color_states["D"]->value = color_names.at("none");
    }
    if(robot.time_since_start >= 42.0){
        robot.color_states["D"]->value = color_names.at("yellow");
    }
    if(robot.time_since_start >= 42.1){
        robot.color_states["D"]->value = color_names.at("none");
    }
}

TaskResult check_task_14(Robot& robot){
    return TaskResult(0, {true});
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