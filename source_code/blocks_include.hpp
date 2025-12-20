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
    auto curr_block = blocks[key];
    if (!curr_block.contains("opcode")) {
        cerr << "Error: Block is missing 'opcode'!" << endl;
        return nullptr;
    }
    auto curr_sequence_block = getFunctionMap()[curr_block["opcode"]](blocks, key).release();
    BlockSequence* block_sequence = new BlockSequence(curr_sequence_block);
    while (true) {
        if (curr_block["next"].is_null() || !blocks.contains(curr_block["next"])) {
            break;
        }
        auto next_block = blocks[curr_block["next"]];
        if (!next_block.contains("opcode")) {
            cerr << "Error: Block is missing 'opcode'!" << endl;
            return nullptr;
        }
        auto next_sequence_block = getFunctionMap()[next_block["opcode"]](blocks, curr_block["next"]).release();
        curr_sequence_block->next = next_sequence_block;
        next_sequence_block->parent = curr_sequence_block;
        curr_sequence_block = next_sequence_block;
        curr_block = next_block;
    }
    return block_sequence;
}


#endif // BLOCKS_H