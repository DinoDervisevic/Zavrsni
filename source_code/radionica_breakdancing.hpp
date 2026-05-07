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
#define MARGIN_OF_ERROR 0.015

#define DEFAULT_SPEED 75

#define MOVED_A_BIT 10

struct TaskResult {
    int score;
    vector<bool> criteria_met;

    TaskResult(int score, vector<bool> criteria_met) : score(score), criteria_met(criteria_met) {
    };
};

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


TaskResult check_task_1_with_motors(Robot& robot, string motor){
    // Kriteriji: smjer (satni ili suprotno);  prešao 360°; dobro postavljen port
    TaskResult result(0, {false, false, false});
    int score = 100;
    bool poceo_kretat = false;
    bool reached_target = false;
    bool halfway = false;

    bool correct_direction = false;

    for(int i = 0; i < robot.robot_states.size(); i++){
        //cout << robot.robot_states[i].motor_states.at(motor) << endl;
        if (!poceo_kretat && check_if_moved_a_bit(robot, motor, i)) {
            poceo_kretat = true;
            score += 0;

            if (robot.robot_states[i].motor_states.at(motor) < 180) {
                correct_direction = true;
            }
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
                score += 0;
            }
        }

        if (reached_target) {
            float pos = robot.robot_states[i].motor_states.at(motor);
            bool in_range = (pos > 360 - MARGIN_OF_ERROR * 360 && pos <= 360) ||
                            (pos < 0 + MARGIN_OF_ERROR * 360 && pos >= 0);
            if (!in_range) {
                score -= 25;
                break;
            }
        }
    }

    if (!reached_target && poceo_kretat) {
        score -= 25;
    }
    if (!poceo_kretat) {
        score -= 50; // Ili je naredba potpuno kriva ili je kriv port
    }

    result.score = score;
    result.criteria_met[0] = correct_direction;
    result.criteria_met[1] = score == 100 ? true : false;
    result.criteria_met[2] = poceo_kretat;
    return result;
}

TaskResult check_task_1_2_2(Robot& robot){
    TaskResult result1 = check_task_1_with_motors(robot, "D");
    TaskResult result2 = check_task_1_with_motors(robot, "F");
    if (result1.score > result2.score){
        return result1;
    } else {
        return result2;
    }
}

TaskResult check_task_2(Robot& robot){
    // Kriteriji: samo 1 motor; oba istovremeno; nisu istovremeno; prvi port dobro postavljen; drugi port dobro postavljen; oba porta dobro postavljena
    TaskResult result(0, {false, false, false, false, false});
    int score = 100;
    bool poceo_kretat_D = false;
    bool poceo_kretat_F = false;
    bool reached_target = false;

    time_t start_time_D = -1;
    time_t start_time_F = -1;

    for(int i = 0; i < robot.robot_states.size(); i++){
        if (!poceo_kretat_D && check_if_moved_a_bit(robot, "D", i)) {
            poceo_kretat_D = true;
            start_time_D = robot.robot_states[i].t;
        }
        if (!poceo_kretat_F && check_if_moved_a_bit(robot, "F", i)) {
            poceo_kretat_F = true;
            start_time_F = robot.robot_states[i].t;
        }
    }

    if (!poceo_kretat_D && !poceo_kretat_F) {
        score -= 100; // Niti jedan motor nije radio
    } else if (!poceo_kretat_D || !poceo_kretat_F) {
        score -= 70; // Samo jedan motor radi
    }

    if (poceo_kretat_D) result.criteria_met[3] = true;
    if (poceo_kretat_F) result.criteria_met[4] = true;

    if (poceo_kretat_D && poceo_kretat_F){
        if (fabs(start_time_D - start_time_F) <= DELTA_T){
            result.criteria_met[1] = true;
        }
        else result.criteria_met[2] = true;
    }
    else if (poceo_kretat_D || poceo_kretat_F){
        result.criteria_met[0] = true;
    }

    result.score = score;
    return result;
}


// Helper funkcija za check_task_3_2 s određenim motorima
TaskResult check_task_3_2_with_motors(Robot& robot, string motor_noge, string motor_ruke){
    
    TaskResult result(0, {false, false, false, false, false, false, false});
    int score = 100;
    
    // === NOGE (375° rotacija uz brzinu 30%) ===
    bool D_started = false;
    bool D_moving_clockwise = false;
    bool D_passed_180 = false;
    bool D_passed_full = false;
    bool D_reached_target = false;
    bool D_finished = false;
    bool D_correct_movement = true;
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

    bool one_speed_after_end = false;
    
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
        
        //cout << "Time: " << time << " | D pos: " << pos_D << " | D speed: " << speed_D << " | D value: " << value_D << endl;

        // --- Praćenje brzine noge (30%) ---
        if (speed_D == 30 && !D_speed_correct){
            D_speed_correct = true;
            D_speed_set_time = time;
        }
        else if(speed_D != DEFAULT_SPEED && !D_speed_correct){
            D_speed_set_time = time; // Postavljena je neka brzina ali nije 30, dakle krivi parametar
        }
        
        // --- Praćenje brzine ruke (50%) ---
        if (speed_F == 50 && !F_speed_correct){
            F_speed_correct = true;
            F_speed_set_time = time;
        }
        else if(speed_F != DEFAULT_SPEED && !F_speed_correct){
            F_speed_set_time = time; // Postavljena je neka brzina ali nije 50, dakle krivi parametar
        }
        
        // === NOGE - 375° ===
        if (!D_started && check_if_moved_a_bit(robot, motor_noge, i)){
            D_started = true;
            D_start_time = time;
            D_moving_clockwise = (pos_D > 180);
            
            if (D_speed_correct && D_speed_set_time <= D_start_time){
                D_speed_set_before_start = true;
            }
            else if(D_speed_set_time > 0 && D_speed_set_time <= D_start_time){
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

            if (D_reached_target){
                bool in_range = (pos_D > TARGET_ANGLE - ANGLE_TOLERANCE && pos_D < TARGET_ANGLE + ANGLE_TOLERANCE) ||
                                (pos_D > 360 - TARGET_ANGLE - ANGLE_TOLERANCE && pos_D < 360 - TARGET_ANGLE + ANGLE_TOLERANCE);
                if (!in_range){
                    D_correct_movement = false;
                }
            }
        }
        
        // === RUKE - 5 sekundi ===
        if (!F_started && check_if_moved_a_bit(robot, motor_ruke, i)){
            F_started = true;
            F_start_time = time;
            
            if (F_speed_correct && F_speed_set_time <= F_start_time){
                F_speed_set_before_start = true;
            }
            else if(F_speed_set_time > 0 && F_speed_set_time <= F_start_time){
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
    
    // 1. Noge - 375° rotacija: -10 za krivi parametar
    if (D_started){
        if (D_reached_target && D_correct_movement){
            result.criteria_met[4] = true;

        } else{
            score -= 10; // Počeo se kretati ali nije došao do cilja
        } 
    }
    
    // 2. Ruke - 5 sekundi: -10 za krivi parametar
    if (F_started && F_finished){
        float F_duration = F_end_time - F_start_time;
        
        if (fabs(F_duration - 5.0) <= TIME_TOLERANCE){
            result.criteria_met[5] = true;
        } else{
            score -= 10;
        } 
    }
    else if (F_started && !F_finished){
        score -= 10; // Počeo se kretati ali nije završio
    }

    if (D_started && F_started){
        result.criteria_met[6] = true;
    }
    else if (D_started || F_started){
        score -= 70; // Ponovo, pretpostavljamo da je tocno namjesten samo 1 motor, a ne da je problem u portu
    }
    else if (!D_started && !F_started){
        score -= 100; // Niti jedan motor nije radio
    }
    
    // 3. Brzina noge (30%): max 20 bodova
    if (D_speed_correct){
        result.criteria_met[2] = true;
        if (!D_speed_set_before_start){
            one_speed_after_end = true;
        }
        else result.criteria_met[0] = true; // Brzina noge postavljena prije početka kretanja
    }
    else{
        score -= 10;
    }
    
    // 4. Brzina ruke (50%): max 20 bodova
    if (F_speed_correct){
        result.criteria_met[3] = true;
        if (!F_speed_set_before_start){
            one_speed_after_end = true;
        }
        else result.criteria_met[1] = true; // Brzina ruke postavljena prije početka kretanja
    }
    else{
        score -= 10;
    }

    if (one_speed_after_end){
        score -= 20;
    }
    
    if (score > 100) score = 100;
    if (score < 0) score = 0;
    
    result.score = score;
    return result;
}

TaskResult check_task_3_2_2(Robot& robot){
    TaskResult result(0, {false, false, false, false, false, false, false});
    int score = 0;
    
    // === Odredi koji motor radi noge (375°) a koji ruke (5s) ===
    // Pretpostavka: motor koji prvi napravi veću rotaciju je "noge", drugi je "ruke"
    // ALI: ako su zamijenili, prihvaćamo obje konfiguracije
    
    // Probaj obje konfiguracije i uzmi bolji rezultat
    TaskResult result1 = check_task_3_2_with_motors(robot, "D", "F");  // D=noge, F=ruke
    TaskResult result2 = check_task_3_2_with_motors(robot, "F", "D");  // F=noge, D=ruke
    
    return (result1.score > result2.score) ? result1 : result2;
}


// Helper funkcija za check_task_4_2 s određenim motorima
TaskResult check_task_4_2_with_motors(Robot& robot, string motor_noge, string motor_ruke){
    TaskResult result(0, {false, false, false, false, false, false, false, false, false});
    int score = 100;
    
    // === NOGE - 3x lijevo-desno (3 × 360°) uz brzinu 20% ===
    bool D_started = false;
    float D_start_time = -1.0;
    float D_end_time = -1.0;
    bool D_finished = false;
    int D_full_rotations = 0;
    bool D_passed_180 = false;
    bool D_movement_correct = true;
    
    // === RUKE - 360° u smjeru kazaljke sata uz brzinu 100% ===
    bool F_started = false;
    float F_start_time = -1.0;
    float F_end_time = -1.0;
    bool F_finished = false;
    bool F_moving_clockwise = false;
    bool F_passed_180 = false;
    bool F_reached_360 = false;
    bool F_movement_correct = true;
    
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
        else if(speed_D != DEFAULT_SPEED && !D_speed_correct){
            D_speed_set_time = time; // Postavljena je neka brzina ali nije 20, dakle krivi parametar
        }
        
        // --- Praćenje brzine ruke (100%) ---
        if (speed_F == 100 && !F_speed_correct){
            F_speed_correct = true;
            F_speed_set_time = time;
        }
        else if(speed_F != DEFAULT_SPEED && !F_speed_correct){
            F_speed_set_time = time; // Postavljena je neka brzina ali nije 100, dakle krivi parametar
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
                    bool accepted = (pos_D > 360 - ANGLE_TOLERANCE) || (pos_D < ANGLE_TOLERANCE);
                    if (!accepted){
                        D_movement_correct = false; // Ako nije na 0 ili 360, ne računamo posljednju rotaciju jer nije dovršena
                    }
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
                bool accepted = (pos_F > 360 - ANGLE_TOLERANCE) || (pos_F < ANGLE_TOLERANCE);
                if (!accepted){
                    F_movement_correct = false;
                }
            }
        }
    }
    
    // === BODOVANJE ===
    
    // 1. Noge - 3 pune rotacije: max 15 bodova
    if (D_started){
        result.criteria_met[6] = true;
        //cout << "D_full_rotations: " << D_full_rotations << endl;
        if (D_full_rotations == 3 && D_movement_correct){
            result.criteria_met[4] = true;
        } else {
            score -= 10;   // Barem se pomaknuo
        }
    }
    
    // 2. Ruke - 360° CW: max 15 bodova
    if (F_started){
        result.criteria_met[7] = true;
        //cout << "F_reached_360: " << F_reached_360 << ", F_moving_clockwise: " << F_moving_clockwise << endl;
        if (F_reached_360 && F_moving_clockwise && F_movement_correct){
            result.criteria_met[5] = true;
            if (F_passed_180) score -= 5;
        } else {
            score -= 10;   // Barem se pomaknuo
        }
    }
    
    // 3. Brzina noge (20%): max 10 bodova
    if (D_speed_correct){
        result.criteria_met[2] = true;
        if (D_speed_set_before_start){
            result.criteria_met[0] = true;
        }
    }
    else{
        score -= 10;
    }
    if (D_speed_set_time > 0 && D_speed_set_time < D_start_time){
        result.criteria_met[0] = true;
    }
    
    // 4. Brzina ruke (100%): max 10 bodova
    if (F_speed_correct){
        result.criteria_met[3] = true;
        if (F_speed_set_before_start){
            result.criteria_met[1] = true;
        }
    }
    else{
        score -= 10;
    }

    // 5. Istovremeni početak
    // Samo ako su oba motora krenula
    if (D_started && F_started && D_start_time > 0 && F_start_time > 0){
        if (fabs(D_start_time - F_start_time) <= TIME_TOLERANCE){
            result.criteria_met[8] = true;
        }
        else{
            score -= 60;
        }
    }
    else if (D_started || F_started){
        score -= 90; // Samo jedan motor je krenuo, pretpostavljamo da nije port kriv
    }

    if ((!D_speed_set_before_start) || (!F_speed_set_before_start)){
        score -= 10;
    }
    
    // Ograniči na max 100
    if (score > 100) score = 100;
    if (score < 0) score = 0;
    
    result.score = score;
    return result;
}

TaskResult check_task_4_2_2(Robot& robot){
    // Probaj obje konfiguracije i uzmi bolji rezultat
    TaskResult result_DF = check_task_4_2_with_motors(robot, "D", "F");  // D=noge, F=ruke
    TaskResult result_FD = check_task_4_2_with_motors(robot, "F", "D");  // F=noge, D=ruke
    return (result_DF.score > result_FD.score) ? result_DF : result_FD;
}


void outside_interference_5(Robot& robot){
    if (!robot.distance_states.count("B") || !robot.distance_states["B"]) return;
    if (robot.time_since_start < 19.1) {
        robot.distance_states["B"]->value = 100.0; // daleko
    } else {
        robot.distance_states["B"]->value = 4.0;   // bliže od 5cm
    }
}


TaskResult check_task_5_2(Robot& robot){
    TaskResult result(0, {false, false, false, false, false, false, false});
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
                        score += 50;  // Oba motora plešu
                        if (robot.robot_states[i].motor_states_speed.at("D") == 50 && robot.robot_states[i].motor_states_speed.at("F") == 50){
                            result.criteria_met[3] = true; // Oba motora imaju ispravnu brzinu
                        }
                        else score -= 10; // Kriva brzina
                    } else {
                        score += 40;  // Samo jedan motor, oduzeto 10 bodova
                    }
                }
                
                // Ako se prepreka pojavila prije nego je robot krenuo plesati
                if (obstacle_close && !motors_active){
                    return result; // Nema bodova, robot se do sada nije ni pomaknuo
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
                    result.score = score; // Daj bodove koje je do sada zaradio, ali ne nastavljaj dalje
                    return result; // Nema bodova, robot je trebao nastaviti plesati dok se ne pojavi prepreka
                }
                break;
                
            case OBSTACLE_DETECTED:
                // Prepreka je detektirana - robot bi trebao stati
                if (!motors_active){
                    stopped_time = time;
                    float reaction_time = stopped_time - obstacle_detected_time;
                    
                    // Bodovi za brzu reakciju
                    if (reaction_time <= REACTION_TOLERANCE){
                        score += 0;  // Brzo je stao
                    } else {
                        score -= 10;  // Stao je, ali sporo
                    }
                    
                    state = STOPPED;
                }
                
                // Ako je prošlo previše vremena a još pleše
                if (motors_active && time - obstacle_detected_time > REACTION_TOLERANCE * 5){
                    // Robot nije reagirao na prepreku - penalizacija
                    result.score = score;
                    return result; // Nema bodova, nije stao
                }
                break;
                
            case STOPPED:
                // Robot je stao - provjeravamo da NE počne opet plesati dok je prepreka tu
                if (motors_active && obstacle_close){
                    // Robot je počeo plesati dok je prepreka još tu - penalizacija
                    score -= 10;
                    result.score = score;
                    return result;
                }
                
                // Prepreka je otišla - provjeravamo da robot OSTANE zaustavljen (zauvijek)
                if (!obstacle_close){
                    if (motors_active){
                        // Robot je počeo plesati nakon što je prepreka otišla - penalizacija
                        // (Zadatak kaže da PRESTANE plesati, ne da nastavi kad prepreka ode)
                        score -= 10;
                        result.score = score;
                        return result;
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
        score += 50;
    }
    
    // Ograniči score na [0, 100]
    if (score < 0) score = 0;
    if (score > 100) score = 100;
    
    result.score = score;
    return result;
}

void outside_interference_6(Robot& robot){
    if (!robot.distance_states.count("B") || !robot.distance_states["B"]) return;

    if (robot.time_since_start < 1.0) {
        robot.distance_states["B"]->value = 100.0;
    } else if (robot.time_since_start < 1.5) {
        robot.distance_states["B"]->value = 5.1;
    } else if (robot.time_since_start < 2.5) {
        robot.distance_states["B"]->value = 4.1; 
    } else if (robot.time_since_start < 200.0) {
        robot.distance_states["B"]->value = 100.0;
    } else if (robot.time_since_start < 200.5) {
        robot.distance_states["B"]->value = 4.0;
    } else {
        robot.distance_states["B"]->value = 100.0;
    }
}


TaskResult check_task_6_2_2(Robot& robot){
    TaskResult result(0, {false, false, false, false, false, false, false, false});
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

    int cycle_count = 0;
    
    const float START_TOLERANCE = 1.5;   // Koliko brzo mora početi nakon pojave prepreke
    
    for(int i = 0; i < robot.robot_states.size(); i++){
        float distance = robot.robot_states[i].distance_states.at("B");
        float value_D = robot.robot_states[i].motor_states_value.at("D");
        float value_F = robot.robot_states[i].motor_states_value.at("F");
        float time = robot.robot_states[i].t;
        
        bool motors_active = (value_D != 0 || value_F != 0);
        bool both_motors_active = (value_D != 0 && value_F != 0);
        bool obstacle_close = (distance <= 5.0);
        
        switch(state){
            case WAITING_FOR_OBSTACLE:
                // Čekamo da se nešto približi robotu
                if (obstacle_close){
                    obstacle_appeared_time = time;
                    state = OBSTACLE_PRESENT;
                    result.criteria_met[0] = true; // Detektirao prepreku
                    result.criteria_met[2] = true;
                    result.criteria_met[4] = true; // Detektirao prepreku (sensor port je dobar)
                }
                if (motors_active && !obstacle_close){
                    // Robot se pokrenuo prije nego se nešto približilo - FAIL
                    started_moving_prematurely = true;
                }
                break;
                
            case OBSTACLE_PRESENT:
                // Prepreka je tu - robot bi se trebao pokrenuti
                if (motors_active){
                    if (both_motors_active){
                        result.criteria_met[3] = true; // Oba motora su se pokrenula
                    }
                    if (robot.robot_states[i].motor_states_value.at("D") != 0){
                        result.criteria_met[6] = true; // Motor D se pokrenuo
                    }
                    if (robot.robot_states[i].motor_states_value.at("F") != 0){
                        result.criteria_met[7] = true; // Motor F se pokrenuo
                    }
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
                    if (abs(stopped_time - movement_start_time - 180) <= 2.0){
                        result.criteria_met[5] = true; // Trajalo 180 sekundi
                    }
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
                cycle_count++;
                state = WAITING_FOR_OBSTACLE;
        }
        
        // Ako je ciklus završen, prekini petlju
        if (cycle_completed){
            break;
        }
    }
    
    if (cycle_count > 1){
        result.criteria_met[1] = true; // Završio ciklus dva puta
    }
    else score -= 40; // Nije imao forever loop

    if (cycle_completed){
        score = 100;  // Puni bodovi za uspješan ciklus
    }  else if (state == MOVING){
        // Robot se pokrenuo ali nije stao do kraja simulacije
        score = 90;  // Djelomični bodovi
    } else {
        // Robot se nije ni pokrenuo
        score = 0;
    }

    if (started_moving_prematurely){
        score -= 10; // Penalizacija za pokretanje prije pojave prepreke
    }

    if (score < 0) score = 0;
    
    result.score = score;
    return result;
}

#endif