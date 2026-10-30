#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <fstream>
#include <algorithm>
#include <functional>
#include <set>
#include <queue>
#include <sstream>

#include "blocks_include.hpp"
#include "motion_simulation.hpp"
#include "radionica.hpp"
//#include "json_export.hpp"

using namespace std;

using json = nlohmann::json;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;


void print_robot_state(Robot& robot) {
    cout << robot.time_since_start << endl;
    robot.print_position();
    //robot.print_display();
    //robot.print_sound();
}

void run_interference(Robot& robot, int taskId){
    switch (taskId) {
        case 3:  outside_interference_task_3(robot); break;
        case 11: outside_interference_task_11(robot); break;
        case 12: outside_interference_task_12(robot); break;
        case 13: outside_interference_task_13(robot); break;
        default: return;
    }
}

void start_simulation(Robot& robot, vector<BlockSequence*> sequences, int taskId) {
    bool done = false;
    int debug_step = 0;
    //cerr << "Starting simulation..." << endl;
    while (true){
        if (taskId != 13){
            if (robot.time_since_start >= 30.0){
                break;
            }
        }
        else{
            if (robot.time_since_start >= 100.0){
                break;
            }
        }
        
        for(auto sequence : sequences){
            sequence->execute(robot);
            if(sequence->get_current_block() != nullptr){
                done = false;
            }
            else {
                sequence->reset(robot);
            }
        }
        run_robot(robot);
        
        //cout << robot.force_states["C"]->value << endl;

        if(taskId != -1){
            adjust_trash_can(robot); // only for workshop
            run_interference(robot, taskId); // only for workshop
        }

        robot.save_state();
        // if (debug_step % 500 == 0) {
        //     cerr << "[DEBUG] t=" << robot.time_since_start
        //         << " force=" << robot.force_states["C"]->value
        //         << " previous_force=" << robot.force_states["C"]->previous_value
        //         << " motor_A_position=" << robot.motor_states["A"]->position
        //         << " motor_A_value=" << robot.motor_states["A"]->value
        //         << " motor_A_current_speed=" << robot.motor_states["A"]->current_speed
        //         << " motor_A_time_left=" << robot.motor_states["A"]->time_left
        //         << endl;
        // }

        debug_step++;

        robot.time_since_start += robot.discrete_time_interval;
    }
}

TaskResult check_if_correct(Robot& robot, int taskId){
    switch (taskId) {
        case 0:  return check_task_0(robot);
        case 1:  return check_task_1(robot);
        case 2:  return check_task_2(robot);
        case 3:  return check_task_3(robot);
        case 4:  return check_task_4(robot);
        case 5:  return check_task_5(robot);
        case 6:  return check_task_6(robot);
        case 7:  return check_task_7(robot);
        case 8:  return check_task_8(robot);
        case 9:  return check_task_9(robot);
        case 10: return check_task_10(robot);
        case 11: return check_task_11(robot);
        case 12: return check_task_12(robot);
        case 13: return check_task_13(robot);
        case 14: return check_task_14(robot);
        default: return TaskResult(0, {false});
    }
}


void print_sequences(vector<BlockSequence*> sequences, Robot& robot) {
    for (const auto& sequence : sequences) {
        Block* block = sequence->get_current_block();
        while (block != nullptr) {
            cout << "Block: " << block->name << ", Opcode: " << block->type << endl;
            cout << "Execution: " << block->execute(robot) << endl;
            block = block->next;
        }
        cout << "End of sequence" << endl;
    }
}



int main(int argc, char* argv[]) {
    //cout << "Starting simulation..." << endl;
    //string json_file_path = "C:/Users/amrad/Downloads/radionica/Radionica File(1)/Radionica File/snapshot_data/project.json";
    //string json_file_path = "C:/Users/amrad/OneDrive/Documents/LEGO Education SPIKE/project.json";
    

    if (argc < 3) {
        cerr << "Usage: simulacija.exe <task_id> <project.json>" << endl;
        return 1;
    }

    int taskId = stoi(argv[1]);
    string json_file_path = argv[2];

    ifstream file(json_file_path);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return -1;
    }

    json j;

    try {
        file >> j;
    } catch (const json::parse_error& error) {
        cerr << "JSON parse error: " << error.what() << endl;
        return 1;
    }

    auto blocks = j["targets"][1]["blocks"];
    vector<BlockSequence*> sequences;
    for(auto it = blocks.begin(); it != blocks.end(); ++it){
        //cerr << "Processing block: " << it.key() << endl;
        if (it.value()["topLevel"]) {
            //cerr << "Found top-level block: " << it.key() << endl;
            string opcode = it.value()["opcode"].get<string>();
            //cerr << "Found top-level block: " << it.key() << endl;
            // Procesiraj samo ako je event blok (početak programa)
            if (opcode.find("event") != string::npos || 
                opcode.find("Event") != string::npos ||
                opcode.find("when") != string::npos ||
                opcode.find("When") != string::npos) {
                    //cerr << "Processing top-level event block: " << it.key() << " with opcode: " << opcode << endl;
                
                BlockSequence* block_sequence = processBlock(blocks, it.key());
                
                if (block_sequence != nullptr) {
                    sequences.push_back(block_sequence);
                } else {
                    cerr << "[WARNING] Preskacem sekvencu: "
                        << it.key() << endl;
                }
            }
            // Ignoriraj sve ostale top-level blokove (nepovezani blokovi)
        }
    }//cerr << "Starting simulation..." << endl;

    Robot robot("robot", 0, 0, sequences);
    robot.addMotorState("A", 75);
    robot.addMotorState("B", 75);
    robot.addForceSensor("C", 0);
    robot.addColorSensor("D", -1);
    
    try {
        start_simulation(robot, sequences, taskId);
    } catch (const ExitProgramException& e) {
        cerr << "Program exited via ControlStop" << endl;
    }
    for (auto& sequence : sequences) {
        delete sequence;
    }
    TaskResult result = check_if_correct(robot, taskId);

    // Exportaj u JSON
    vector<Obstacle> obstacles;  // Za sada prazno — kasnije učitaj iz task.json
    vector<Wall> walls;           // Za sada prazno — kasnije učitaj iz task.json
    //export_simulation_to_json(robot.robot_states, obstacles, walls, "simulation_result.json");

    cout << result.score << endl;
    for (bool met : result.criteria_met) {
        cout << (met ? "true" : "false") << endl;
    }

    return 0;

}
