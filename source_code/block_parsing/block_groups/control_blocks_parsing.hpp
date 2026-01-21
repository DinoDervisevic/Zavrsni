#ifndef CONTROL_BLOCKS_PARSING_HPP
#define CONTROL_BLOCKS_PARSING_HPP

#include <map>
#include <functional>
#include <memory>
#include <string>

#include "../../blocks/Block.hpp"
#include "../../blocks/BlockSequence.hpp"
#include "../../external/json.hpp"
#include "../../blocks/control_blocks/control_blocks_include.hpp"
#include "../../blocks/blank_blocks/blank_blocks_include.hpp"
#include "../string_conversion.hpp"

using namespace std;
using json = nlohmann::json;

// U control_blocks_parsing.hpp (ili drugdje gdje trebaš)
class BlockSequence;
BlockSequence* processBlock(const json& blocks, std::string key);

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;

FunctionMap createControlFunctionMap(FunctionMap& globalMap) {
    FunctionMap functionMap;

     // Control blocks
    functionMap["control_wait"] = [&globalMap](const json& json_object, const string& name) {
        Block* duration;
        if(json_object[name]["inputs"]["DURATION"][0] == 1){
            if (json_object[name]["inputs"]["DURATION"][1][1].get<string>() == ""){
                duration = new BlankBlockDouble(0.0);
            }
            else duration = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["DURATION"][1][1].get<string>()));
        } else {
            auto duration_name = json_object[name]["inputs"]["DURATION"][1];
            duration = globalMap[json_object[duration_name]["opcode"]](json_object, duration_name).release();
        }
        return make_unique<Wait>(duration);
    };

    functionMap["control_repeat"] = [&globalMap](const json& json_object, const string& name) {
        Block* times;
        if(json_object[name]["inputs"]["TIMES"][0] == 1){
            if (json_object[name]["inputs"]["TIMES"][1][1].get<string>() == ""){
                times = new BlankBlockInt(0);
            }
            else times = new BlankBlockInt(parseInt(json_object[name]["inputs"]["TIMES"][1][1].get<string>()));
        } else {
            auto times_name = json_object[name]["inputs"]["TIMES"][1];
            times = globalMap[json_object[times_name]["opcode"]](json_object, times_name).release();
        }

        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            auto substack_input = json_object[name]["inputs"]["SUBSTACK"];
            if (substack_input.size() >= 2 && !substack_input[1].is_null()){
                auto key = json_object[name]["inputs"]["SUBSTACK"][1];
                block_sequence = processBlock(json_object, key);
            } 
        }

        return make_unique<Repeat>(times, block_sequence);
    };

    functionMap["control_forever"] = [&globalMap](const json& json_object, const string& name) {
        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            auto substack_input = json_object[name]["inputs"]["SUBSTACK"];
            if (substack_input.size() >= 2 && !substack_input[1].is_null()){
                auto key = json_object[name]["inputs"]["SUBSTACK"][1];
                block_sequence = processBlock(json_object, key);
            } 
        }

        return make_unique<Forever>(block_sequence);
    };

    functionMap ["control_if"] = [&globalMap](const json& json_object, const string& name) {
        Block* condition = nullptr;
        if (json_object[name]["inputs"].contains("CONDITION")) {
            auto cond_input = json_object[name]["inputs"]["CONDITION"];
            if (cond_input.size() >= 2 && !cond_input[1].is_null()) {
                auto condition_name = cond_input[1];
                condition = globalMap[json_object[condition_name]["opcode"]](json_object, condition_name).release();
            }
        }

        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            auto substack_input = json_object[name]["inputs"]["SUBSTACK"];
            if (substack_input.size() >= 2 && !substack_input[1].is_null()){
                auto key = json_object[name]["inputs"]["SUBSTACK"][1];
                block_sequence = processBlock(json_object, key);
            } 
        }

        return make_unique<If>(block_sequence, condition);
    };

    functionMap["control_if_else"] = [&globalMap](const json& json_object, const string& name) {
        Block* condition = nullptr;
        if (json_object[name]["inputs"].contains("CONDITION")) {
            auto cond_input = json_object[name]["inputs"]["CONDITION"];
            if (cond_input.size() >= 2 && !cond_input[1].is_null()) {
                auto condition_name = cond_input[1];
                condition = globalMap[json_object[condition_name]["opcode"]](json_object, condition_name).release();
            }
        }

        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            auto substack_input = json_object[name]["inputs"]["SUBSTACK"];
            if (substack_input.size() >= 2 && !substack_input[1].is_null()){
                auto key = json_object[name]["inputs"]["SUBSTACK"][1];
                block_sequence = processBlock(json_object, key);
            } 
        }

        BlockSequence* else_block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK2")) {
            auto substack_input = json_object[name]["inputs"]["SUBSTACK2"];
            if (substack_input.size() >= 2 && !substack_input[1].is_null()){
                auto key = json_object[name]["inputs"]["SUBSTACK2"][1];
                else_block_sequence = processBlock(json_object, key);
            } 
        }

        return make_unique<IfElse>(block_sequence, else_block_sequence, condition);
    };

    functionMap ["control_wait_until"] = [&globalMap](const json& json_object, const string& name) {
        Block* condition = nullptr;
        if (json_object[name]["inputs"].contains("CONDITION")) {
            auto cond_input = json_object[name]["inputs"]["CONDITION"];
            if (cond_input.size() >= 2 && !cond_input[1].is_null()) {
                auto condition_name = cond_input[1];
                condition = globalMap[json_object[condition_name]["opcode"]](json_object, condition_name).release();
            }
        }

        return make_unique<WaitUntil>(condition);
    };

    functionMap ["control_repeat_until"] = [&globalMap](const json& json_object, const string& name) {
        Block* condition = nullptr;
        if (json_object[name]["inputs"].contains("CONDITION")) {
            auto cond_input = json_object[name]["inputs"]["CONDITION"];
            if (cond_input.size() >= 2 && !cond_input[1].is_null()) {
                auto condition_name = cond_input[1];
                condition = globalMap[json_object[condition_name]["opcode"]](json_object, condition_name).release();
            }
        }

        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            auto substack_input = json_object[name]["inputs"]["SUBSTACK"];
            if (substack_input.size() >= 2 && !substack_input[1].is_null()){
                auto key = json_object[name]["inputs"]["SUBSTACK"][1];
                block_sequence = processBlock(json_object, key);
            } 
        }

        return make_unique<RepeatUntil>(condition, block_sequence);
    };

    functionMap ["flippercontrol_stopOtherStacks"] = [&globalMap](const json& json_object, const string& name) {
        return make_unique<StopOtherStacks>();
    };

    functionMap ["flippercontrol_stop"] = [&globalMap](const json& json_object, const string& name) {
        auto option = json_object[name]["fields"]["STOP_OPTION"][0];
        return make_unique<ControlStop>(option);
    };
    //--------------------------------------------


    return functionMap;

}


#endif