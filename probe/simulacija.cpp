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
#include "block_sequence.hpp"

using namespace std;

using json = nlohmann::json;

double time_since_start = 0; // current time in seconds

FunctionMap functionMap = createFunctionMap();



void start_simulation(Robot& robot, json blocks) {
}

void print_sequences(vector<BlockSequence*> sequences) {
    for (const auto& sequence : sequences) {
        Block* block = sequence->get_start_block();
        while (block != nullptr) {
            cout << "Block: " << block->name << ", Opcode: " << block->type << endl;
            block = block->next;
        }
        cout << "End of sequence" << endl;
    }
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

    Robot robot("robot", 0, 0);

    vector<BlockSequence*> sequences;
    for(auto it = blocks.begin(); it != blocks.end(); ++it){
        if(it.value()["topLevel"]) {
            auto curr_block = it.value();
            auto curr_sequence_block = functionMap[it.value()["opcode"]](blocks, it.key()).release();
            BlockSequence* block_sequence = new BlockSequence(curr_sequence_block);
            sequences.push_back(block_sequence);
            while(true){
                if(curr_block["next"].is_null()){
                    break;
                }
                
                auto next_block = blocks[curr_block["next"]];
                auto next_sequence_block = functionMap[next_block["opcode"]](blocks, curr_block["next"]).release();
                curr_sequence_block->next = next_sequence_block;
                next_sequence_block->parent = curr_sequence_block;
                curr_sequence_block = next_sequence_block;
                curr_block = next_block;
                
            }
        }
    }
    

    print_sequences(sequences);

    return 0;
}