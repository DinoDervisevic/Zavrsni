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
#include "radionica_ljiljana.hpp"

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
        case 1: outside_interference(robot); break;
        case 2: outside_interference(robot); break;
        case 3: outside_interference(robot); break;
        case 4: outside_interference(robot); break;
        case 5: outside_interference_5(robot); break;
        case 6: outside_interference_6(robot); break;
        default: return;
    }
}

void start_simulation(Robot& robot, vector<BlockSequence*> sequences, int taskId) {
    bool done = false;
    while (true){
        if (taskId != 6){
            if (robot.time_since_start >= 30.0){
                break;
            }
        }
        if (taskId == 6){
            if (robot.time_since_start >= 400.0){
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

        // Ovdje nam ne treba racunanje micanja robota jer se robot ne mice, potrebno nam je samo izracunati stanja motora u svakom koraku
        calculate_motor_speed(robot);
        subtract_time_from_motors(robot);

        for(auto i : "ABCDEF"){
            robot.calculate_motor_position(string(1, i));
        }

        calculate_previous_value(robot);

        //cout << "Time: " << robot.time_since_start << " ";
        //cout << robot.motor_states["D"]->value << " " << robot.motor_states["F"]->value << endl;
        robot.save_state();
        //print_robot_state(robot);

        run_interference(robot, taskId);

        robot.time_since_start += robot.discrete_time_interval;
    }
}

int check_if_correct(Robot& robot, int taskId){
    switch (taskId) {
        case 1: return check_task_1(robot);
        case 2: return check_task_2(robot);
        case 3: return check_task_3(robot);
        case 4: return check_task_4(robot);
        case 5: return check_task_5(robot);
        case 6: return check_task_6(robot);
        default: return 0;
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
    //string json_file_path = "C:/Users/amrad/OneDrive/Desktop/fer/4_1/seminar_za_ljiljanu/koraci1/extraction_folder/project.json";

    if (argc < 3) {
        std::cerr << "Please provide the task ID as a command line argument." << std::endl;
        return -1;
    }

    string json_file_path = argv[2];

    ifstream file(json_file_path);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return -1;
    }

    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return -1;
    }

    auto blocks = j["targets"][1]["blocks"];
    vector<BlockSequence*> sequences;
    for(auto it = blocks.begin(); it != blocks.end(); ++it){
        if (it.value()["topLevel"]) {
            BlockSequence* block_sequence = processBlock(blocks, it.key());
            sequences.push_back(block_sequence);
        }
    }

    Robot robot("robot", 0, 0, sequences);
    robot.addMotorState("D", 75);
    robot.addMotorState("F", 75);
    robot.addDistanceSensor("B", 0);

    int taskId = std::stoi(argv[1]);
    
    start_simulation(robot, sequences, taskId);
    for (auto& sequence : sequences) {
        delete sequence;
    }
    int result = check_if_correct(robot, taskId);

    cout << result << endl;

    return 0;
}
