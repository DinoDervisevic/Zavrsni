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


void start_simulation(Robot& robot, vector<BlockSequence*> sequences) {
    while (true){
        for(auto sequence : sequences){ //TODO: make the logic for paralel block sequence interpretation
            sequence->execute(robot);
        }
        run_robot(robot);
        robot.time_since_start += robot.discrete_time_interval;

        print_robot_state(robot);
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

void print_robot_state(Robot& robot) {
    cout << robot.time_since_start << endl;
    robot.print_position();
    //robot.print_display();
    //robot.print_sound();
}



int main() {
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
    
    print_sequences(sequences, robot);

    return 0;

}
