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
        score += 20;
        if (!D_speed_set_before_start){
            one_speed_after_end = true;
        }
    }
    
    // 4. Brzina ruke (50%): max 20 bodova
    if (F_speed_correct){
        score += 20;
        if (!F_speed_set_before_start){
            one_speed_after_end = true;
        }
    }

    if (one_speed_after_end){
        score -= 10;
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
        score += 10;
    }
    
    // 4. Brzina ruke (100%): max 10 bodova
    if (F_speed_correct){
        score += 10;
    }

    // 5. Istovremeni početak: 50 bodova
    // Samo ako su oba motora krenula
    if (D_started && F_started && D_start_time > 0 && F_start_time > 0){
        if (fabs(D_start_time - F_start_time) <= TIME_TOLERANCE){
            score += 50;
        }
    }

    if ((D_speed_correct && !D_speed_set_before_start) || (F_speed_correct && !F_speed_set_before_start)){
        score -= 10;
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
        score -= 100; // Penalizacija za pokretanje prije pojave prepreke
    }

    if (score < 0) score = 0;
    
    return score;
}

#endif