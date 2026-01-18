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



int check_task_1_with_motors(Robot& robot, string motor){
    int score = 0;
    bool poceo_kretat = false;
    bool reached_target = false;
    bool halfway = false;

    for(int i = 0; i < robot.robot_states.size(); i++){
        //cout << robot.robot_states[i].motor_states.at(motor) << endl;
        if (!poceo_kretat && check_if_moved_a_bit(robot, motor, i)) {
            poceo_kretat = true;
            score += 30;
        }

        if (poceo_kretat && !halfway) {
            float pos = robot.robot_states[i].motor_states.at(motor);
            if ((pos > 180 - MARGIN_OF_ERROR * 360 && pos < 180 + MARGIN_OF_ERROR * 360)) {
                halfway = true;
            }
        }

        if (poceo_kretat && !reached_target && halfway) {
            float pos = robot.robot_states[i].motor_states.at(motor);
            if ((pos > 360 - MARGIN_OF_ERROR * 360 && pos < 360) ||
                (pos < 0 + MARGIN_OF_ERROR * 360 && pos > 0)) {
                reached_target = true;
                score += 70;
            }
        }

        if (reached_target) {
            float pos = robot.robot_states[i].motor_states.at(motor);
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

int check_task_1_2_2(Robot& robot){
    return max(check_task_1_with_motors(robot, "D"), check_task_1_with_motors(robot, "F"));
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

int check_task_3_2(Robot& robot){
    int score = 0;
    
    // === NOGE (Motor D) - 375° rotacija uz brzinu 30% ===
    // 375° = 1 puna rotacija (360°) + 15° = završava na poziciji 15° (ili 345° ako ide CCW)
    bool D_started = false;
    bool D_moving_clockwise = false;
    bool D_passed_180 = false;      // Prošao pola kruga
    bool D_passed_full = false;     // Prošao puni krug (360°)
    bool D_reached_target = false;  // Stigao na 375° (pozicija 15° ili 345°)
    bool D_finished = false;
    float D_start_time = -1.0;
    float D_end_time = -1.0;
    
    // === RUKE (Motor F) - 5 sekundi kretanja uz brzinu 50% ===
    bool F_started = false;
    bool F_finished = false;
    float F_start_time = -1.0;
    float F_end_time = -1.0;
    
    // === Brzine ===
    bool D_speed_set_before_start = false;
    bool D_speed_set_after_end = false;
    bool D_speed_correct = false;
    float D_speed_set_time = -1.0;
    
    bool F_speed_set_before_start = false;
    bool F_speed_set_after_end = false;
    bool F_speed_correct = false;
    float F_speed_set_time = -1.0;
    
    const float TARGET_ANGLE = 15.0;  // 375° mod 360° = 15°
    const float ANGLE_TOLERANCE = MARGIN_OF_ERROR * 360;  // ~10.8°
    const float TIME_TOLERANCE = 0.5;  // 0.5s tolerancija za trajanje
    
    for(int i = 0; i < robot.robot_states.size(); i++){
        float time = robot.robot_states[i].t;
        float pos_D = robot.robot_states[i].motor_states.at("D");
        float pos_F = robot.robot_states[i].motor_states.at("F");
        float speed_D = robot.robot_states[i].motor_states_speed.at("D");
        float speed_F = robot.robot_states[i].motor_states_speed.at("F");
        float value_D = robot.robot_states[i].motor_states_value.at("D");
        float value_F = robot.robot_states[i].motor_states_value.at("F");
        
        // --- Praćenje brzine D (30%) ---
        if (speed_D == 30 && !D_speed_correct){
            D_speed_correct = true;
            D_speed_set_time = time;
        }
        
        // --- Praćenje brzine F (50%) ---
        if (speed_F == 50 && !F_speed_correct){
            F_speed_correct = true;
            F_speed_set_time = time;
        }
        
        // === NOGE (Motor D) - 375° ===
        if (!D_started && check_if_moved_a_bit(robot, "D", i)){
            D_started = true;
            D_start_time = time;
            
            // Odredi smjer
            D_moving_clockwise = (pos_D > 180);
            
            // Provjeri je li brzina bila namještena PRIJE početka kretanja
            if (D_speed_correct && D_speed_set_time <= D_start_time){
                D_speed_set_before_start = true;
            }
        }
        
        // Praćenje napretka rotacije D
        if (D_started && !D_finished){
            // Provjeri prolazak kroz 180°
            if (!D_passed_180){
                if (pos_D > 180 - ANGLE_TOLERANCE && pos_D < 180 + ANGLE_TOLERANCE){
                    D_passed_180 = true;
                }
            }
            
            // Provjeri prolazak kroz 360°/0° (puni krug)
            if (D_passed_180 && !D_passed_full){
                if ((pos_D > 360 - ANGLE_TOLERANCE) || (pos_D < ANGLE_TOLERANCE)){
                    D_passed_full = true;
                }
            }
            
            // Provjeri je li stigao na ciljnu poziciju (375° = 15° ili -375° = 345°)
            if (D_passed_full && !D_reached_target){
                bool at_15 = (pos_D > TARGET_ANGLE - ANGLE_TOLERANCE && pos_D < TARGET_ANGLE + ANGLE_TOLERANCE);
                bool at_345 = (pos_D > 360 - TARGET_ANGLE - ANGLE_TOLERANCE && pos_D < 360 - TARGET_ANGLE + ANGLE_TOLERANCE);
                
                if (at_15 || at_345){
                    D_reached_target = true;
                }
            }
            
            // Provjeri je li motor stao
            if (D_reached_target && value_D == 0){
                D_finished = true;
                D_end_time = time;
            }
        }
        
        // === RUKE (Motor F) - 5 sekundi ===
        if (!F_started && check_if_moved_a_bit(robot, "F", i)){
            F_started = true;
            F_start_time = time;
            
            // Provjeri je li brzina bila namještena PRIJE početka kretanja
            if (F_speed_correct && F_speed_set_time <= F_start_time){
                F_speed_set_before_start = true;
            }
        }
        
        // Praćenje trajanja kretanja F
        if (F_started && !F_finished){
            if (value_F == 0){
                F_finished = true;
                F_end_time = time;
            }
        }
    }
    
    // Provjeri je li brzina namještena tek NAKON završetka kretanja
    if (D_speed_correct && D_finished && D_speed_set_time > D_end_time){
        D_speed_set_after_end = true;
    }
    if (F_speed_correct && F_finished && F_speed_set_time > F_end_time){
        F_speed_set_after_end = true;
    }
    
    // === BODOVANJE ===
    
    // 1. Noge (Motor D) - 375° rotacija: max 30 bodova
    if (D_started){
        if (D_reached_target){
            score += 30;  // Puni bodovi za ispravnu rotaciju
        } else if (D_passed_full){
            score += 20;  // Prošao puni krug ali nije stao na pravom mjestu
        } else if (D_passed_180){
            score += 10;  // Barem pola kruga
        } else {
            score += 5;   // Barem se pomaknuo
        }
    }
    
    // 2. Ruke (Motor F) - 5 sekundi: max 30 bodova
    if (F_started && F_finished){
        float F_duration = F_end_time - F_start_time;
        
        if (fabs(F_duration - 5.0) <= TIME_TOLERANCE){
            score += 30;  // Puni bodovi za točno 5 sekundi
        } else if (F_duration >= 4.0 && F_duration <= 6.0){
            score += 20;  // Blizu 5 sekundi
        } else if (F_duration >= 3.0 && F_duration <= 7.0){
            score += 10;  // Barem nešto blizu
        } else {
            score += 5;   // Barem se pomaknuo
        }
    } else if (F_started){
        score += 5;  // Krenuo ali nije završio (možda još traje)
    }
    
    // 3. Brzina nogu (30%): max 20 bodova
    if (D_speed_correct){
        if (D_speed_set_before_start){
            score += 20;  // Namješteno prije početka - puni bodovi
        } else if (D_speed_set_after_end){
            score += 10;  // Namješteno nakon završetka - umanjeni bodovi (-10)
        } else {
            score += 15;  // Namješteno tijekom kretanja - djelomični bodovi
        }
    }
    
    // 4. Brzina ruku (50%): max 20 bodova
    if (F_speed_correct){
        if (F_speed_set_before_start){
            score += 20;  // Namješteno prije početka - puni bodovi
        } else if (F_speed_set_after_end){
            score += 10;  // Namješteno nakon završetka - umanjeni bodovi (-10)
        } else {
            score += 15;  // Namješteno tijekom kretanja - djelomični bodovi
        }
    }
    
    // Ograniči na max 100
    if (score > 100) score = 100;
    if (score < 0) score = 0;
    
    return score;
}


// Helper funkcija za check_task_3_2 s određenim motorima
int check_task_3_2_with_motors(Robot& robot, string motor_noge, string motor_ruke){
    int score = 0;
    
    // === NOGE (375° rotacija uz brzinu 30%) ===
    bool D_started = false;
    bool D_moving_clockwise = false;
    bool D_passed_180 = false;
    bool D_passed_full = false;
    bool D_reached_target = false;
    bool D_finished = false;
    float D_start_time = -1.0;
    float D_end_time = -1.0;
    
    // === RUKE (5 sekundi kretanja uz brzinu 50%) ===
    bool F_started = false;
    bool F_finished = false;
    float F_start_time = -1.0;
    float F_end_time = -1.0;
    
    // === Brzine ===
    bool D_speed_set_before_start = false;
    bool D_speed_set_after_end = false;
    bool D_speed_correct = false;
    float D_speed_set_time = -1.0;
    
    bool F_speed_set_before_start = false;
    bool F_speed_set_after_end = false;
    bool F_speed_correct = false;
    float F_speed_set_time = -1.0;
    
    const float TARGET_ANGLE = 15.0;
    const float ANGLE_TOLERANCE = MARGIN_OF_ERROR * 360;
    const float TIME_TOLERANCE = 0.5;
    
    for(int i = 0; i < robot.robot_states.size(); i++){
        float time = robot.robot_states[i].t;
        float pos_D = robot.robot_states[i].motor_states.at(motor_noge);
        float pos_F = robot.robot_states[i].motor_states.at(motor_ruke);
        float speed_D = robot.robot_states[i].motor_states_speed.at(motor_noge);
        float speed_F = robot.robot_states[i].motor_states_speed.at(motor_ruke);
        float value_D = robot.robot_states[i].motor_states_value.at(motor_noge);
        float value_F = robot.robot_states[i].motor_states_value.at(motor_ruke);
        
        // --- Praćenje brzine noge (30%) ---
        if (speed_D == 30 && !D_speed_correct){
            D_speed_correct = true;
            D_speed_set_time = time;
        }
        
        // --- Praćenje brzine ruke (50%) ---
        if (speed_F == 50 && !F_speed_correct){
            F_speed_correct = true;
            F_speed_set_time = time;
        }
        
        // === NOGE - 375° ===
        if (!D_started && check_if_moved_a_bit(robot, motor_noge, i)){
            D_started = true;
            D_start_time = time;
            D_moving_clockwise = (pos_D > 180);
            
            if (D_speed_correct && D_speed_set_time <= D_start_time){
                D_speed_set_before_start = true;
            }
        }
        
        if (D_started && !D_finished){
            if (!D_passed_180){
                if (pos_D > 180 - ANGLE_TOLERANCE && pos_D < 180 + ANGLE_TOLERANCE){
                    D_passed_180 = true;
                }
            }
            
            if (D_passed_180 && !D_passed_full){
                if ((pos_D > 360 - ANGLE_TOLERANCE) || (pos_D < ANGLE_TOLERANCE)){
                    D_passed_full = true;
                }
            }
            
            if (D_passed_full && !D_reached_target){
                bool at_15 = (pos_D > TARGET_ANGLE - ANGLE_TOLERANCE && pos_D < TARGET_ANGLE + ANGLE_TOLERANCE);
                bool at_345 = (pos_D > 360 - TARGET_ANGLE - ANGLE_TOLERANCE && pos_D < 360 - TARGET_ANGLE + ANGLE_TOLERANCE);
                
                if (at_15 || at_345){
                    D_reached_target = true;
                }
            }
            
            if (D_reached_target && value_D == 0){
                D_finished = true;
                D_end_time = time;
            }
        }
        
        // === RUKE - 5 sekundi ===
        if (!F_started && check_if_moved_a_bit(robot, motor_ruke, i)){
            F_started = true;
            F_start_time = time;
            
            if (F_speed_correct && F_speed_set_time <= F_start_time){
                F_speed_set_before_start = true;
            }
        }
        
        if (F_started && !F_finished){
            if (value_F == 0){
                F_finished = true;
                F_end_time = time;
            }
        }
    }
    
    if (D_speed_correct && D_finished && D_speed_set_time > D_end_time){
        D_speed_set_after_end = true;
    }
    if (F_speed_correct && F_finished && F_speed_set_time > F_end_time){
        F_speed_set_after_end = true;
    }
    
    // === BODOVANJE ===
    
    // 1. Noge - 375° rotacija: max 30 bodova
    if (D_started){
        if (D_reached_target){
            score += 30;
        } else if (D_passed_full){
            score += 20;
        } else if (D_passed_180){
            score += 10;
        } else {
            score += 5;
        }
    }
    
    // 2. Ruke - 5 sekundi: max 30 bodova
    if (F_started && F_finished){
        float F_duration = F_end_time - F_start_time;
        
        if (fabs(F_duration - 5.0) <= TIME_TOLERANCE){
            score += 30;
        } else if (F_duration >= 4.0 && F_duration <= 6.0){
            score += 20;
        } else if (F_duration >= 3.0 && F_duration <= 7.0){
            score += 10;
        } else {
            score += 5;
        }
    } else if (F_started){
        score += 5;
    }
    
    // 3. Brzina noge (30%): max 20 bodova
    if (D_speed_correct){
        if (D_speed_set_before_start){
            score += 20;
        } else if (D_speed_set_after_end){
            score += 10;
        } else {
            score += 15;
        }
    }
    
    // 4. Brzina ruke (50%): max 20 bodova
    if (F_speed_correct){
        if (F_speed_set_before_start){
            score += 20;
        } else if (F_speed_set_after_end){
            score += 10;
        } else {
            score += 15;
        }
    }
    
    if (score > 100) score = 100;
    if (score < 0) score = 0;
    
    return score;
}

int check_task_3_2_2(Robot& robot){
    int score = 0;
    
    // === Odredi koji motor radi noge (375°) a koji ruke (5s) ===
    // Pretpostavka: motor koji prvi napravi veću rotaciju je "noge", drugi je "ruke"
    // ALI: ako su zamijenili, prihvaćamo obje konfiguracije
    
    // Probaj obje konfiguracije i uzmi bolji rezultat
    int score_DF = check_task_3_2_with_motors(robot, "D", "F");  // D=noge, F=ruke
    int score_FD = check_task_3_2_with_motors(robot, "F", "D");  // F=noge, D=ruke
    
    return max(score_DF, score_FD);
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

int check_task_4_2(Robot& robot){
    int score = 0;
    
    // === NOGE (Motor D) - 3x lijevo-desno (3 × 360° = 1080°) uz brzinu 20% ===
    bool D_started = false;
    float D_start_time = -1.0;
    float D_end_time = -1.0;
    bool D_finished = false;
    int D_full_rotations = 0;        // Broj punih rotacija (360°)
    bool D_passed_180 = false;       // Za praćenje prolaska kroz pola kruga
    
    // === RUKE (Motor F) - 360° u smjeru kazaljke sata uz brzinu 100% ===
    bool F_started = false;
    float F_start_time = -1.0;
    float F_end_time = -1.0;
    bool F_finished = false;
    bool F_moving_clockwise = false;
    bool F_passed_180 = false;
    bool F_reached_360 = false;
    
    // === Brzine ===
    bool D_speed_correct = false;
    float D_speed_set_time = -1.0;
    bool D_speed_set_before_start = false;
    bool D_speed_set_after_end = false;
    
    bool F_speed_correct = false;
    float F_speed_set_time = -1.0;
    bool F_speed_set_before_start = false;
    bool F_speed_set_after_end = false;
    
    const float ANGLE_TOLERANCE = MARGIN_OF_ERROR * 360;  // ~10.8°
    const float TIME_TOLERANCE = 0.5;
    
    for(int i = 0; i < robot.robot_states.size(); i++){
        float time = robot.robot_states[i].t;
        float pos_D = robot.robot_states[i].motor_states.at("D");
        float pos_F = robot.robot_states[i].motor_states.at("F");
        float speed_D = robot.robot_states[i].motor_states_speed.at("D");
        float speed_F = robot.robot_states[i].motor_states_speed.at("F");
        float value_D = robot.robot_states[i].motor_states_value.at("D");
        float value_F = robot.robot_states[i].motor_states_value.at("F");

        //cout << "Time: " << time << " | D pos: " << pos_D << " | F pos: " << pos_F << endl;
        
        // --- Praćenje brzine D (20%) ---
        if (speed_D == 20 && !D_speed_correct){
            D_speed_correct = true;
            D_speed_set_time = time;
        }
        
        // --- Praćenje brzine F (100%) ---
        if (speed_F == 100 && !F_speed_correct){
            F_speed_correct = true;
            F_speed_set_time = time;
        }
        
        // === NOGE (Motor D) - 3 × 360° ===
        if (!D_started && check_if_moved_a_bit(robot, "D", i)){
            D_started = true;
            D_start_time = time;
            
            // Provjeri je li brzina bila namještena PRIJE početka
            if (D_speed_correct && D_speed_set_time <= D_start_time){
                D_speed_set_before_start = true;
            }
        }
        
        // Praćenje punih rotacija D
        if (D_started && !D_finished){
            // Provjeri prolazak kroz 180°
            if (!D_passed_180){
                if (pos_D > 180 - ANGLE_TOLERANCE && pos_D < 180 + ANGLE_TOLERANCE){
                    D_passed_180 = true;
                }
            }
            
            // Provjeri povratak na 0°/360° (završena puna rotacija)
            if (D_passed_180){
                if ((pos_D > 360 - ANGLE_TOLERANCE) || (pos_D < ANGLE_TOLERANCE)){
                    D_full_rotations++;
                    D_passed_180 = false;  // Resetiraj za sljedeću rotaciju
                }
            }
            
            // Provjeri je li motor stao
            if (value_D == 0 && D_full_rotations > 0){
                D_end_time = time;
            }

            if (value_D == 0 && time - D_end_time >= TIME_TOLERANCE * 3){
                D_finished = true;
            }

            if (D_end_time > 0 && value_D != 0){
                D_end_time = -1.0; // Resetiraj ako se motor ponovno pokrene
            }
        }
        
        // === RUKE (Motor F) - 360° u smjeru kazaljke sata ===
        if (!F_started && check_if_moved_a_bit(robot, "F", i)){
            F_started = true;
            F_start_time = time;
            
            // Odredi smjer (CW = pozicija se smanjuje, ide 360 -> 0)
            F_moving_clockwise = (pos_F > 180);
            
            // Provjeri je li brzina bila namještena PRIJE početka
            if (F_speed_correct && F_speed_set_time <= F_start_time){
                F_speed_set_before_start = true;
            }
        }
        
        // Praćenje rotacije F
        if (F_started && !F_finished){
            // Provjeri prolazak kroz 180°
            if (!F_passed_180){
                if (pos_F > 180 - ANGLE_TOLERANCE && pos_F < 180 + ANGLE_TOLERANCE){
                    F_passed_180 = true;
                }
            }
            
            // Provjeri povratak na početnu poziciju (360°/0°)
            if (F_passed_180 && !F_reached_360){
                if ((pos_F > 360 - ANGLE_TOLERANCE) || (pos_F < ANGLE_TOLERANCE)){
                    F_reached_360 = true;
                }
            }
            
            // Provjeri je li motor stao
            if (F_reached_360 && value_F == 0){
                F_finished = true;
                F_end_time = time;
            }

            if (F_reached_360 && value_F != 0){
                if(pos_F > 180 - ANGLE_TOLERANCE && pos_F < 180 + ANGLE_TOLERANCE){
                    F_finished = true;
                    score -= 15;
                }
            }

        }
    }
    
    // Provjeri je li brzina namještena tek NAKON završetka kretanja
    if (D_speed_correct && D_finished && D_speed_set_time > D_end_time){
        D_speed_set_after_end = true;
    }
    if (F_speed_correct && F_finished && F_speed_set_time > F_end_time){
        F_speed_set_after_end = true;
    }
    
    // === BODOVANJE ===
    
    // 1. Noge (Motor D) - 3 pune rotacije: max 30 bodova
    if (D_started){
        //cout << "D_full_rotations: " << D_full_rotations << endl;
        if (D_full_rotations >= 3 && D_full_rotations <= 4){
            score += 30;  // Puni bodovi - 3 rotacije
        } 
        else {
            score += 15;   // Barem se pomaknuo
        }
    }
    
    // 2. Ruke (Motor F) - 360° CW: max 30 bodova
    if (F_started){
        if (F_reached_360){
            if (F_moving_clockwise){
                score += 30;  // Puni bodovi - točna rotacija i smjer
            } else {
                score += 20;  // Točna rotacija ali krivi smjer
            }
        } else if (F_passed_180){
            score += 15;  // Barem pola kruga
        } else {
            score += 5;   // Barem se pomaknuo
        }
    }
    
    // 3. Brzina nogu (20%): max 20 bodova
    if (D_speed_correct){
        if (D_speed_set_before_start){
            score += 20;  // Namješteno prije početka
        } else if (D_speed_set_after_end){
            score += 10;  // Namješteno nakon završetka (-10)
        } else {
            score += 15;  // Namješteno tijekom kretanja
        }
    }
    
    // 4. Brzina ruku (100%): max 20 bodova
    if (F_speed_correct){
        if (F_speed_set_before_start){
            score += 20;  // Namješteno prije početka
        } else if (F_speed_set_after_end){
            score += 10;  // Namješteno nakon završetka (-10)
        } else {
            score += 15;  // Namješteno tijekom kretanja
        }
    }
    
    // Ograniči na max 100
    if (score > 100) score = 100;
    if (score < 0) score = 0;
    
    return score;
}



// Helper funkcija za check_task_4_2 s određenim motorima
int check_task_4_2_with_motors(Robot& robot, string motor_noge, string motor_ruke){
    int score = 0;
    
    // === NOGE - 3x lijevo-desno (3 × 360°) uz brzinu 20% ===
    bool D_started = false;
    float D_start_time = -1.0;
    float D_end_time = -1.0;
    bool D_finished = false;
    int D_full_rotations = 0;
    bool D_passed_180 = false;
    
    // === RUKE - 360° u smjeru kazaljke sata uz brzinu 100% ===
    bool F_started = false;
    float F_start_time = -1.0;
    float F_end_time = -1.0;
    bool F_finished = false;
    bool F_moving_clockwise = false;
    bool F_passed_180 = false;
    bool F_reached_360 = false;
    
    // === Brzine ===
    bool D_speed_correct = false;
    float D_speed_set_time = -1.0;
    bool D_speed_set_before_start = false;
    
    bool F_speed_correct = false;
    float F_speed_set_time = -1.0;
    bool F_speed_set_before_start = false;
    
    const float ANGLE_TOLERANCE = MARGIN_OF_ERROR * 360;
    const float TIME_TOLERANCE = 0.5;
    
    for(int i = 0; i < robot.robot_states.size(); i++){
        float time = robot.robot_states[i].t;
        float pos_D = robot.robot_states[i].motor_states.at(motor_noge);
        float pos_F = robot.robot_states[i].motor_states.at(motor_ruke);
        float speed_D = robot.robot_states[i].motor_states_speed.at(motor_noge);
        float speed_F = robot.robot_states[i].motor_states_speed.at(motor_ruke);
        float value_D = robot.robot_states[i].motor_states_value.at(motor_noge);
        float value_F = robot.robot_states[i].motor_states_value.at(motor_ruke);
        //cout << "Time: " << time << " | D pos: " << pos_D << " | F pos: " << pos_F << endl;
        
        // --- Praćenje brzine noge (20%) ---
        if (speed_D == 20 && !D_speed_correct){
            D_speed_correct = true;
            D_speed_set_time = time;
        }
        
        // --- Praćenje brzine ruke (100%) ---
        if (speed_F == 100 && !F_speed_correct){
            F_speed_correct = true;
            F_speed_set_time = time;
        }
        
        // === NOGE - 3 × 360° ===
        if (!D_started && check_if_moved_a_bit(robot, motor_noge, i)){
            D_started = true;
            D_start_time = time;
            
            if (D_speed_correct && D_speed_set_time <= D_start_time){
                D_speed_set_before_start = true;
            }
        }
        
        if (D_started && !D_finished){
            if (!D_passed_180){
                if (pos_D > 180 - ANGLE_TOLERANCE && pos_D < 180 + ANGLE_TOLERANCE){
                    D_passed_180 = true;
                }
            }
            
            if (D_passed_180){
                if ((pos_D > 360 - ANGLE_TOLERANCE) || (pos_D < ANGLE_TOLERANCE)){
                    D_full_rotations++;
                    D_passed_180 = false;
                }
            }
            
            if (value_D == 0 && D_full_rotations > 0){
                if (D_end_time < 0) D_end_time = time;
                
                if (time - D_end_time >= TIME_TOLERANCE * 3){
                    D_finished = true;
                }
            }

            if (D_end_time > 0 && value_D != 0){
                D_end_time = -1.0;
            }
        }
        
        // === RUKE - 360° CW ===
        if (!F_started && check_if_moved_a_bit(robot, motor_ruke, i)){
            F_started = true;
            F_start_time = time;
            F_moving_clockwise = (pos_F > 180);
            
            if (F_speed_correct && F_speed_set_time <= F_start_time){
                F_speed_set_before_start = true;
            }
        }
        
        if (F_started && !F_finished){
            if (!F_passed_180){
                if (pos_F > 180 - ANGLE_TOLERANCE && pos_F < 180 + ANGLE_TOLERANCE){
                    F_passed_180 = true;
                }
            }
            
            if (F_passed_180 && !F_reached_360){
                if ((pos_F > 360 - ANGLE_TOLERANCE) || (pos_F < ANGLE_TOLERANCE)){
                    F_reached_360 = true;
                }
            }
            
            if (F_reached_360 && value_F == 0){
                F_finished = true;
                F_passed_180 = false;
                F_end_time = time;
            }
        }
    }
    
    // === BODOVANJE ===
    
    // 1. Noge - 3 pune rotacije: max 15 bodova
    if (D_started){
        //cout << "D_full_rotations: " << D_full_rotations << endl;
        if (D_full_rotations >= 3 && D_full_rotations <= 4){
            score += 15;  // Puni bodovi - napravljen zadatak
        } else if (D_full_rotations >= 1 && D_full_rotations < 8){
            score += 10;  // Barem jedna rotacija
        } else {
            score += 5;   // Barem se pomaknuo
        }
    }
    
    // 2. Ruke - 360° CW: max 15 bodova
    if (F_started){
        //cout << "F_reached_360: " << F_reached_360 << ", F_moving_clockwise: " << F_moving_clockwise << endl;
        if (F_reached_360 && F_moving_clockwise){
            score += 15;  // Puni bodovi - točna rotacija i smjer
            if (F_passed_180) score -= 5;
        } else if (F_reached_360 && !F_passed_180){
            score += 10;  // Točna rotacija ali krivi smjer
        } else if (F_passed_180){
            score += 5;   // Barem pola kruga
        } else {
            score += 2;   // Barem se pomaknuo
        }
    }
    
    // 3. Brzina noge (20%): max 10 bodova
    if (D_speed_correct){
        //cout << "D_speed_correct: true" << endl;
        if (D_speed_set_before_start){
            score += 10;  // Puni bodovi - namješteno prije početka
        } else {
            score += 5;   // Namješteno, ali ne prije početka
        }
    }
    
    // 4. Brzina ruke (100%): max 10 bodova
    if (F_speed_correct){
        //cout << "F_speed_correct: true" << endl;
        if (F_speed_set_before_start){
            score += 10;  // Puni bodovi - namješteno prije početka
        } else {
            score += 5;   // Namješteno, ali ne prije početka
        }
    }

    // 5. Istovremeni početak: 50 bodova
    // Samo ako su oba motora krenula
    if (D_started && F_started && D_start_time > 0 && F_start_time > 0){
        if (fabs(D_start_time - F_start_time) <= TIME_TOLERANCE){
            score += 50;
        }
    }
    
    // Ograniči na max 100
    if (score > 100) score = 100;
    if (score < 0) score = 0;
    
    return score;
}

int check_task_4_2_2(Robot& robot){
    // Probaj obje konfiguracije i uzmi bolji rezultat
    int score_DF = check_task_4_2_with_motors(robot, "D", "F");  // D=noge, F=ruke
    int score_FD = check_task_4_2_with_motors(robot, "F", "D");  // F=noge, D=ruke
    
    return max(score_DF, score_FD);
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

int check_task_5_2(Robot& robot){
    int score = 0;
    
    enum State { WAITING_TO_START, DANCING, OBSTACLE_DETECTED, STOPPED };
    State state = WAITING_TO_START;
    
    float dance_start_time = -1.0;
    float obstacle_detected_time = -1.0;
    float stopped_time = -1.0;
    bool both_motors_dancing = false;
    
    const float REACTION_TOLERANCE = 1.0;    // Koliko brzo mora stati nakon detekcije prepreke
    const float SHORT_PAUSE_TOLERANCE = 0.5; // Tolerancija za kratke pauze između blokova u while petlji
    
    float last_motor_active_time = -1.0;     // Zadnje vrijeme kad su motori bili aktivni
    
    for(int i = 0; i < robot.robot_states.size(); i++){
        float distance = robot.robot_states[i].distance_states.at("B");
        float value_D = robot.robot_states[i].motor_states_value.at("D");
        float value_F = robot.robot_states[i].motor_states_value.at("F");
        float time = robot.robot_states[i].t;

        //cout << "Time: " << time << " Distance: " << distance << " Motor D value: " << value_D << " Motor F value: " << value_F << endl;
        
        bool motors_active = (value_D != 0 || value_F != 0);
        bool both_motors_active = (value_D != 0 && value_F != 0);
        bool obstacle_close = (distance <= 5.0);

        //cout << "Time: " << time << " State: " << state << " Motors active: " << motors_active << " Obstacle close: " << obstacle_close << " Score: " << score << endl;
        
        // Prati zadnje vrijeme kad su motori bili aktivni (za toleranciju kratkih pauza)
        if (motors_active){
            last_motor_active_time = time;
        }
        
        switch(state){
            case WAITING_TO_START:
                // Čekamo da robot počne plesati (prije nego se pojavi prepreka)
                if (motors_active && !obstacle_close){
                    dance_start_time = time;
                    both_motors_dancing = both_motors_active;
                    state = DANCING;
                    
                    // Bodovi za početak plesa
                    if (both_motors_active){
                        score += 30;  // Oba motora plešu
                    } else {
                        score += 10;  // Samo jedan motor
                    }
                }
                
                // Ako se prepreka pojavila prije nego je robot krenuo plesati
                if (obstacle_close && !motors_active){
                    return score; // Nema bodova, robot se do sada nije ni pomaknuo
                }
                break;
                
            case DANCING:
                // Robot pleše - provjeri je li se pojavila prepreka
                if (obstacle_close){
                    obstacle_detected_time = time;
                    state = OBSTACLE_DETECTED;
                }
                
                // Ako motori stanu na kratko (pauza između blokova), to je OK
                if (!motors_active && time - last_motor_active_time > SHORT_PAUSE_TOLERANCE){
                    // Robot je stao plesati bez razloga - penalizacija
                    return score; // Nema bodova, robot je trebao nastaviti plesati dok se ne pojavi prepreka
                }
                break;
                
            case OBSTACLE_DETECTED:
                // Prepreka je detektirana - robot bi trebao stati
                if (!motors_active){
                    stopped_time = time;
                    float reaction_time = stopped_time - obstacle_detected_time;
                    
                    // Bodovi za brzu reakciju
                    if (reaction_time <= REACTION_TOLERANCE){
                        score += 40;  // Brzo je stao
                    } else {
                        score += 30;  // Stao je, ali sporo
                    }
                    
                    state = STOPPED;
                }
                
                // Ako je prošlo previše vremena a još pleše
                if (motors_active && time - obstacle_detected_time > REACTION_TOLERANCE * 5){
                    // Robot nije reagirao na prepreku - penalizacija
                    return score; // Nema bodova, nije stao
                }
                break;
                
            case STOPPED:
                // Robot je stao - provjeravamo da NE počne opet plesati dok je prepreka tu
                if (motors_active && obstacle_close){
                    // Robot je počeo plesati dok je prepreka još tu - penalizacija
                    score -= 20;
                    return score;
                }
                
                // Prepreka je otišla - provjeravamo da robot OSTANE zaustavljen (zauvijek)
                if (!obstacle_close){
                    if (motors_active){
                        // Robot je počeo plesati nakon što je prepreka otišla - penalizacija
                        // (Zadatak kaže da PRESTANE plesati, ne da nastavi kad prepreka ode)
                        score -= 10;
                        return score;
                    } else {
                        // Robot je ostao zaustavljen - dobro!
                        // (Bodovi se daju samo jednom, na kraju)
                    }
                }
                break;
        }
    }
    
    // Bonus bodovi ako je cijeli scenarij prošao dobro
    if (state == STOPPED && stopped_time > 0){
        // Robot je uspješno stao i ostao zaustavljen
        score += 30;
    }
    
    // Ograniči score na [0, 100]
    if (score < 0) score = 0;
    if (score > 100) score = 100;
    
    return score;
}

void outside_interference_6(Robot& robot){
    if (!robot.distance_states.count("B") || !robot.distance_states["B"]) return;

    if (robot.time_since_start < 1.0) {
        robot.distance_states["B"]->value = 100.0;
    } else if (robot.time_since_start < 1.5) {
        robot.distance_states["B"]->value = 4.0;
    } else if (robot.time_since_start < 200.0) {
        robot.distance_states["B"]->value = 100.0;
    } else if (robot.time_since_start < 200.5) {
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


int check_task_6_2(Robot& robot){
    int score = 0;
    
    // Stanje za praćenje svakog ciklusa (prepreka se pojavi -> robot pleše 3 min -> stane)
    enum State { WAITING_FOR_OBSTACLE, OBSTACLE_PRESENT, MOVING, SHOULD_STOP };
    State state = WAITING_FOR_OBSTACLE;
    
    float obstacle_appeared_time = -1.0;
    float movement_start_time = -1.0;
    bool both_motors_started = false;
    int completed_cycles = 0;
    
    const float DANCE_DURATION = 180.0;  // 3 minute
    const float START_TOLERANCE = 1.0;   // Tolerancija za početak kretanja
    const float STOP_TOLERANCE = 5.0;    // Tolerancija za zaustavljanje
    const float START_BUFFER = 0.05;      // Dodatna tolerancija za početak kretanja
    
    for(int i = 0; i < robot.robot_states.size(); i++){
        float distance = robot.robot_states[i].distance_states.at("B");
        float value_D = robot.robot_states[i].motor_states_value.at("D");
        float value_F = robot.robot_states[i].motor_states_value.at("F");
        float time = robot.robot_states[i].t;

        //cout << "Time: " << time << " Distance: " << distance << " Motor D value: " << value_D << " Motor F value: " << value_F << endl;
        
        bool motors_active = (value_D != 0 || value_F != 0);
        bool both_motors_active = (value_D != 0 && value_F != 0);
        bool obstacle_close = (distance <= 5.0);
        
        switch(state){
            case WAITING_FOR_OBSTACLE:
                // Penalizacija ako se robot kreće prije pojave prepreke
                if (motors_active && !obstacle_close){
                    // Robot se kreće kad ne bi trebao - ne dajemo bodove za ovaj ciklus
                }
                
                // Prepreka se pojavila
                if (obstacle_close){
                    obstacle_appeared_time = time;
                    state = OBSTACLE_PRESENT;
                    //cout << "Obstacle appeared at time: " << obstacle_appeared_time << "at distance: " << distance << endl;
                }
                break;
                
            case OBSTACLE_PRESENT:
                // Čekamo da robot počne plesati nakon što se prepreka pojavila
                if (motors_active){
                    // Ako je ovo prvi frame kad su motori aktivni, zapamti vrijeme
                    if (movement_start_time < 0.0){
                        movement_start_time = time;
                        both_motors_started = both_motors_active;
                    }
                    
                    // Čekaj malo prije nego damo bodove (da se oba motora uspiju upaliti)
                    if (time - obstacle_appeared_time >= START_BUFFER){
                        // Provjeri je li krenuo dovoljno brzo nakon pojave prepreke
                        if (movement_start_time - obstacle_appeared_time <= START_TOLERANCE){
                            if (both_motors_started){
                                score += 25;  // Oba motora krenula na vrijeme
                                //cout << "Both motors started dancing at time: " << movement_start_time << endl;
                            } else {
                                score += 25;  // Samo jedan motor krenuo
                                //cout << "One motor started dancing at time: " << movement_start_time << endl;
                            }
                        }
                        //cout << "Robot started dancing at time: " << movement_start_time  << " after obstacle appeared at: " << obstacle_appeared_time << endl;
                        state = MOVING;
                    }
                }
                
                // Ako prepreka ode prije nego robot krene, resetiraj
                if (!obstacle_close && time - obstacle_appeared_time > START_TOLERANCE){
                    state = WAITING_FOR_OBSTACLE;
                    obstacle_appeared_time = -1.0;
                    movement_start_time = -1.0;
                    //cout << "Obstacle disappeared before dancing started at time: " << time << endl;
                }
                break;
                
            case MOVING:
                // Robot pleše - provjeri traje li dovoljno dugo
                if (time - movement_start_time >= DANCE_DURATION - STOP_TOLERANCE){
                    state = SHOULD_STOP;
                }
                else if (!motors_active){
                    // Robot se zaustavio prerano
                    state = WAITING_FOR_OBSTACLE;
                    obstacle_appeared_time = -1.0;
                    movement_start_time = -1.0;
                    both_motors_started = false;
                }
                break;
                
            case SHOULD_STOP:
                // Robot bi se trebao zaustaviti oko 180s nakon početka
                if (!motors_active){
                    float dance_duration = time - movement_start_time;
                    
                    // Provjeri je li plesao približno 3 minute
                    if (fabs(dance_duration - DANCE_DURATION) <= STOP_TOLERANCE){
                        score += 25;  // Zaustavio se u pravo vrijeme
                        //cout << "Robot stopped dancing at time: " << time << " after dancing for " << dance_duration << " seconds." << endl;
                    }
                    
                    completed_cycles++;
                    
                    // Resetiraj za sljedeći ciklus
                    state = WAITING_FOR_OBSTACLE;
                    obstacle_appeared_time = -1.0;
                    movement_start_time = -1.0;
                    both_motors_started = false;
                }
                
                // Ako je prošlo previše vremena a još se kreće, penaliziraj i resetiraj
                if (time - movement_start_time > DANCE_DURATION + STOP_TOLERANCE * 2){
                    state = WAITING_FOR_OBSTACLE;
                    obstacle_appeared_time = -1.0;
                    movement_start_time = -1.0;
                    both_motors_started = false;
                }
                break;
        }
    }
    

    if (score > 100) score = 100;
    
    return score;
}

int check_task_6_2_2(Robot& robot){
    int score = 0;
    
    // Stanja:
    // 1. WAITING_FOR_OBSTACLE - čekamo da se nešto približi
    // 2. OBSTACLE_PRESENT - prepreka je tu, robot bi se trebao pokrenuti
    // 3. MOVING - robot se kreće, čekamo da stane
    // 4. STOPPED - robot je stao, provjeravamo da ostane zaustavljen dok se opet nešto ne približi
    // 5. CYCLE_COMPLETE - jedan ciklus je uspješno završen
    
    enum State { WAITING_FOR_OBSTACLE, OBSTACLE_PRESENT, MOVING, STOPPED, CYCLE_COMPLETE };
    State state = WAITING_FOR_OBSTACLE;
    
    float obstacle_appeared_time = -1.0;
    float movement_start_time = -1.0;
    float stopped_time = -1.0;
    bool cycle_completed = false;
    bool started_moving_prematurely = false;
    
    const float START_TOLERANCE = 1.5;   // Koliko brzo mora početi nakon pojave prepreke
    
    for(int i = 0; i < robot.robot_states.size(); i++){
        float distance = robot.robot_states[i].distance_states.at("B");
        float value_D = robot.robot_states[i].motor_states_value.at("D");
        float value_F = robot.robot_states[i].motor_states_value.at("F");
        float time = robot.robot_states[i].t;
        
        bool motors_active = (value_D != 0 || value_F != 0);
        bool obstacle_close = (distance <= 5.0);
        
        switch(state){
            case WAITING_FOR_OBSTACLE:
                // Čekamo da se nešto približi robotu
                if (obstacle_close){
                    obstacle_appeared_time = time;
                    state = OBSTACLE_PRESENT;
                }
                if (motors_active && !obstacle_close){
                    // Robot se pokrenuo prije nego se nešto približilo - FAIL
                    started_moving_prematurely = true;
                }
                break;
                
            case OBSTACLE_PRESENT:
                // Prepreka je tu - robot bi se trebao pokrenuti
                if (motors_active){
                    movement_start_time = time;
                    
                    // Provjeri je li krenuo dovoljno brzo
                    if (movement_start_time - obstacle_appeared_time <= START_TOLERANCE){
                        // Dobro je krenuo!
                        state = MOVING;
                    } else {
                        // Prekasno je krenuo - ali svejedno pratimo
                        state = MOVING;
                    }
                }
                
                // Ako prepreka ode prije nego robot krene, resetiraj
                if (!obstacle_close && !motors_active){
                    state = WAITING_FOR_OBSTACLE;
                    obstacle_appeared_time = -1.0;
                }
                break;
                
            case MOVING:
                // Robot se kreće - čekamo da stane
                if (!motors_active){
                    stopped_time = time;
                    state = STOPPED;
                }
                break;
                
            case STOPPED:
                // Robot je stao - provjeravamo da OSTANE zaustavljen dok se opet nešto ne približi
                
                if (obstacle_close){
                    // Nešto se opet približilo - ciklus je uspješno završen!
                    // (ne zahtijevamo da se robot opet pokrene)
                    cycle_completed = true;
                    state = CYCLE_COMPLETE;
                }
                
                if (motors_active && !obstacle_close){
                    // Robot se pokrenuo prije nego se nešto približilo - FAIL
                    // Resetiraj i pokušaj opet
                    state = WAITING_FOR_OBSTACLE;
                    obstacle_appeared_time = -1.0;
                    movement_start_time = -1.0;
                    stopped_time = -1.0;
                }
                break;
                
            case CYCLE_COMPLETE:
                // Ciklus je završen, možemo izaći
                break;
        }
        
        // Ako je ciklus završen, prekini petlju
        if (cycle_completed){
            break;
        }
    }
    
    // Bodovanje:
    // - Ako je završio barem jedan ciklus: 100 bodova
    // - Ako je stao ali nije dočekao drugu prepreku (simulacija završila): djelomični bodovi
    // - Ako se pokrenuo ali nije stao: mali bodovi
    // - Ako se nije ni pokrenuo: 0 bodova
    
    if (cycle_completed){
        score = 100;  // Puni bodovi za uspješan ciklus
    } else if (state == STOPPED){
        // Robot je stao i ostao zaustavljen do kraja simulacije
        // (možda simulacija nije trajala dovoljno dugo da se pojavi druga prepreka)
        score = 80;  // Većina bodova - napravio je sve što je mogao
    } else if (state == MOVING){
        // Robot se pokrenuo ali nije stao do kraja simulacije
        score = 40;  // Djelomični bodovi
    } else if (movement_start_time > 0){
        // Robot se pokrenuo ali je negdje zapeo
        score = 30;
    } else {
        // Robot se nije ni pokrenuo
        score = 0;
    }

    if (started_moving_prematurely){
        score -= 50; // Penalizacija za pokretanje prije pojave prepreke
    }

    if (score < 0) score = 0;
    
    return score;
}

#endif