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
#include "json_export.hpp"

using namespace std;

using json = nlohmann::json;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;


void print_robot_state(Robot& robot) {
    cout << robot.time_since_start << endl;
    robot.print_position();
    //robot.print_display();
    //robot.print_sound();
}


void start_simulation(Robot& robot, vector<BlockSequence*> sequences, map<string, Obstacle>& obstacles, map<string, Wall>& walls) {
    bool done = false;
    while (true){
        if(robot.time_since_start >= 10.0){
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
        run_robot(robot, obstacles, walls);
        robot.save_state();
        print_robot_state(robot);
        //cout << robot.force_states["C"]->value << endl;

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


void read_obstacles_and_walls(const string& file_path, map<string, 
    Obstacle>& obstacles, map<string, Wall>& walls, Robot& robot) {
    if (file_path == "") return;
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Could not open the file!" << endl;
        return;
    }

    json j;
    file >> j;

    for(auto& obj : j["objects"]) {
        if (obj["type"] == "obstacle") {
            obstacles.emplace(obj["id"], Obstacle{obj["x"], -(double)obj["y"], obj["width"], obj["length"], obj["angle"]});
        } else if (obj["type"] == "wall") {
            walls.emplace(obj["id"], Wall{obj["x1"], -(double)obj["y1"], obj["x2"], -(double)obj["y2"]});
        }
        else if (obj["type"] == "robot") {
            robot.x = obj["x"];
            robot.y = -(double)obj["y"];
            robot.angle = obj["angle"];
        }
    }
}



int main(int argc, char* argv[]) {
    cout << "Starting simulation..." << endl;
    //string json_file_path = "C:/Users/amrad/Downloads/radionica/Radionica File(1)/Radionica File/snapshot_data/project.json";
    string json_file_path = argc > 1 ? argv[1] : "C:/Users/amrad/OneDrive/Documents/LEGO Education SPIKE/project.json";
    
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

    string task_json_path = argc > 2 ? argv[2] : "";

    map<string, Obstacle> obstacles;
    map<string, Wall> walls;
    read_obstacles_and_walls(task_json_path, obstacles, walls, robot);
    
    try {
        start_simulation(robot, sequences, obstacles, walls);
    } catch (const ExitProgramException& e) {
        cerr << "Program exited via ControlStop" << endl;
    }
    for (auto& sequence : sequences) {
        delete sequence;
    }

    // Export to JSON
    string output_path = argc > 3 ? argv[3] : "simulation_result.json";
    export_simulation_to_json(robot.robot_states, obstacles, walls, output_path);

    return 0;

}
