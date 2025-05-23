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
#include "robot.hpp"
#include "blocks.hpp"
#include "functions.hpp"
#include "motion_simulation.hpp"

using namespace std;

using json = nlohmann::json;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;


void print_robot_state(Robot& robot) {
    cout << robot.time_since_start << endl;
    robot.print_position();
    robot.print_display();
    robot.print_sound();
}

void start_simulation(Robot& robot, vector<BlockSequence*> sequences) {
    bool done = false;
    while (true){
        if(robot.time_since_start >= 5.0){
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
        print_robot_state(robot);

        robot.time_since_start += robot.discrete_time_interval;
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



int main() {
    cout << "Starting simulation..." << endl;
    string json_file_path = "C:/Users/amrad/OneDrive/Documents/LEGO Education SPIKE/project.json";

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
    robot.addColorSensor("D", 0);

    
    start_simulation(robot, sequences);
    for (auto& sequence : sequences) {
        delete sequence;
    }

    cout << "Simulation finished." << endl;

    return 0;

}
