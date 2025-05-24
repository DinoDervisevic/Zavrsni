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

#include "json.hpp"
#include "robot/robot.hpp"
#include "blocks.hpp"
#include "functions.hpp"
#include "motion_simulation.hpp"
#include "radionica.hpp"

using namespace std;

using json = nlohmann::json;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;


void print_robot_state(Robot& robot) {
    cout << robot.time_since_start << endl;
    robot.print_position();
    robot.print_display();
    //robot.print_sound();
}

void run_interference(Robot& robot, int taskId){
    switch (taskId) {
        case 1:  outside_interference_task_3(robot); break;
        case 2:  outside_interference_task_3(robot); break;
        case 3:  outside_interference_task_3(robot); break;
        case 4:  outside_interference_task_3(robot); break;
        case 5:  outside_interference_task_3(robot); break;
        case 6:  outside_interference_task_3(robot); break;
        case 7:  outside_interference_task_3(robot); break;
        case 8:  outside_interference_task_3(robot); break;
        case 9:  outside_interference_task_3(robot); break;
        case 10: outside_interference_task_3(robot); break;
        case 11: outside_interference_task_11(robot); break;
        case 12: outside_interference_task_12(robot); break;
        case 13: outside_interference_task_13(robot); break;
        default: return;
    }
}

void start_simulation(Robot& robot, vector<BlockSequence*> sequences, int taskId) {
    bool done = false;
    while (true){
        if(robot.time_since_start >= 40.0){
            break;
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
        robot.save_state();
        //print_robot_state(robot);
        //cout << robot.force_states["C"]->value << endl;

        adjust_trash_can(robot); // only for workshop
        run_interference(robot, taskId); // only for workshop

        robot.time_since_start += robot.discrete_time_interval;
    }
}

bool check_if_correct(Robot& robot, int taskId){
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
        default: return false;
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
    string json_file_path = "C:/Users/amrad/Downloads/radionica/Radionica File(1)/Radionica File/snapshot_data/project.json";

    ifstream file(json_file_path);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return -1;
    }

    json j;
    file >> j;
    auto blocks = j["targets"][1]["blocks"];

    vector<BlockSequence*> sequences;
    for(auto it = blocks.begin(); it != blocks.end(); ++it){
        if (it.value()["topLevel"]) {
            BlockSequence* block_sequence = processBlock(blocks, it.key());
            sequences.push_back(block_sequence);
        }
    }

    Robot robot("robot", 0, 0, sequences);

    robot.addMotorState("A", 75);
    robot.addMotorState("B", 75);
    robot.addForceSensor("C", 0);
    robot.addColorSensor("D", -1);

    int taskId = std::stoi(argv[1]);
    
    start_simulation(robot, sequences, taskId);
    for (auto& sequence : sequences) {
        delete sequence;
    }
    bool result = check_if_correct(robot, taskId);

    cout << result << endl;

    return 0;

}
