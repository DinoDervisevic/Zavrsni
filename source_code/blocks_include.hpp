#ifndef BLOCKS_H
#define BLOCKS_H

#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <map>
#include <cctype>
#include <memory>
#include <functional>
#include <algorithm>
#include <random>
#include <cctype>
#include <cmath>
#include <iomanip>

#include "external/json.hpp"
#include "robot/Robot.hpp"
#include "utils/functions.hpp"
#include "utils/display_letters.hpp"
#include "blocks/common_block_includes.hpp"
#include "blocks/all_blocks_include.hpp"
#include "block_parsing/functionMap.hpp"

using namespace std;

using json = nlohmann::json;

inline FunctionMap& getFunctionMap() {
    static FunctionMap functionMap = createFunctionMap();
    return functionMap;
}

inline BlockSequence* processBlock(const json& blocks, string key) {
    //cerr << "Processing block: " << key << endl;
    auto curr_block = blocks[key];
    if (!curr_block.contains("opcode")) {
        cerr << "Error: Block is missing 'opcode'!" << endl;
        return nullptr;
    }
    //cerr << "  Opcode: " << curr_block["opcode"] << endl;
    Block* curr_sequence_block = nullptr;
    try {
        curr_sequence_block = getFunctionMap()[curr_block["opcode"]](blocks, key).release();
    } catch (const std::exception& e) {
    cerr << "[BLOCK ERROR] opcode="
         << curr_block["opcode"]
         << ", block=" << key
         << ", error=" << e.what()
         << endl;
    return nullptr;
}
    //cerr << "  Block created successfully." << endl;
    if (curr_sequence_block == nullptr) return nullptr;
    BlockSequence* block_sequence = new BlockSequence(curr_sequence_block);
    while (true) {
        //cerr << "  Opcode: " << curr_block["opcode"] << endl;
        //cerr << "  Full block JSON:\n" << curr_block.dump(2) << endl;
        if (curr_block["next"].is_null() || !blocks.contains(curr_block["next"])) {
            break;
        }
        auto next_block = blocks[curr_block["next"]];
        if (!next_block.contains("opcode")) {
            cerr << "Error: Block is missing 'opcode'!" << endl;
            return nullptr;
        }
        
        Block* next_sequence_block = nullptr;

        try {
            next_sequence_block =
                getFunctionMap()[next_block["opcode"]]
                (blocks, curr_block["next"])
                .release();
        } catch (const std::exception& e) {
            cerr << "[BLOCK ERROR] opcode="
                << next_block["opcode"]
                << ", block=" << curr_block["next"]
                << ", error=" << e.what()
                << endl;

            delete block_sequence;
            return nullptr;
        }

        if (next_sequence_block == nullptr) {
            cerr << "[BLOCK ERROR] Block nije kreiran: "
                << curr_block["next"] << endl;

            delete block_sequence;
            return nullptr;
        }
        curr_sequence_block->next = next_sequence_block;
        next_sequence_block->parent = curr_sequence_block;
        curr_sequence_block = next_sequence_block;
        curr_block = next_block;
    }
    return block_sequence;
}


#endif // BLOCKS_H