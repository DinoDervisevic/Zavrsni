#ifndef MOVEMENT_BLOCKS_PARSING_HPP
#define MOVEMENT_BLOCKS_PARSING_HPP

#include <map>
#include <functional>
#include <memory>
#include <string>

#include "../../blocks/Block.hpp"
#include "../../blocks/BlockSequence.hpp"
#include "../../external/json.hpp"
#include "../../blocks/movement_blocks/movement_blocks_include.hpp"
#include "../../blocks/blank_blocks/blank_blocks_include.hpp"

using namespace std;
using json = nlohmann::json;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;

FunctionMap createMovementFunctionMap(FunctionMap& globalMap) {
    FunctionMap functionMap;

     // Movement blocks
    functionMap["flippermove_move"] = [&globalMap](const json& json_object, const string& name) {
        bool forward;
        int args = json_object[name]["inputs"]["DIRECTION"][0];
        if(args != 1){
            forward = true;
        }
        else{
            string direction_name = json_object[name]["inputs"]["DIRECTION"][1];
            string fwd = json_object[direction_name]["fields"]["field_flippermove_custom-icon-direction"][0];
            
            if(fwd == "forward") {
                forward = true;
            } else {
                forward = false;
            }
        }
        
        string unit = json_object[name]["fields"]["UNIT"][0];

        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            value = globalMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }

        return make_unique<Move>(forward, value, unit);
    };

    functionMap["flippermove_startMove"] = [&globalMap](const json& json_object, const string& name) {
        bool forward;
        int args = json_object[name]["inputs"]["DIRECTION"][0];
        if(args != 1){
            forward = true;
        }
        else{
            string direction_name = json_object[name]["inputs"]["DIRECTION"][1];
            string fwd = json_object[direction_name]["fields"]["field_flippermove_custom-icon-direction"][0];
            
            if(fwd == "forward") {
                forward = true;
            } else {
                forward = false;
            }
        }
        return make_unique<StartMove>(forward);
    };

    functionMap["flippermove_rotation-wheel"] = [&globalMap](const json& json_object, const string& name) {
        string direction = json_object[name]["fields"]["field_flippermove_rotation-wheel"][0];
        int value = stoi(direction.substr(direction.find(" ") + 1));
        return make_unique<BlankBlockInt>(value);
    };

    functionMap["flippermove_steer"] = [&globalMap](const json& json_object, const string& name) {
        string direction_name = json_object[name]["inputs"]["STEERING"][1];
        Block* direction = globalMap[json_object[direction_name]["opcode"]](json_object, direction_name).release();
        
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            value = globalMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        
        string unit = json_object[name]["fields"]["UNIT"][0];
        return make_unique<Steer>(direction, value, unit);
    };

    functionMap["flippermove_startSteer"] = [&globalMap](const json& json_object, const string& name) {
        string direction_name = json_object[name]["inputs"]["STEERING"][1];
        Block* direction = globalMap[json_object[direction_name]["opcode"]](json_object, direction_name).release();
        return make_unique<StartSteer>(direction);
    };

    functionMap["flippermove_stopMove"] = [&globalMap](const json& json_object, const string& name) {
        return make_unique<StopMoving>();
    };

    functionMap["flippermove_movementSpeed"] = [&globalMap](const json& json_object, const string& name) {
        Block* speed;
        if(json_object[name]["inputs"]["SPEED"][0] == 1){
            speed = new BlankBlockDouble(stod(json_object[name]["inputs"]["SPEED"][1][1].get<string>()));
        } else {
            string speed_name = json_object[name]["inputs"]["SPEED"][1];
            speed = globalMap[json_object[speed_name]["opcode"]](json_object, speed_name).release();
        }
        return make_unique<SetMovementSpeed>(speed);
    };

    functionMap["flippermove_setMovementPair"] = [&globalMap](const json& json_object, const string& name) {
        string pair_name = json_object[name]["inputs"]["PAIR"][1];
        Block* pair = globalMap[json_object[pair_name]["opcode"]](json_object, pair_name).release();
        return make_unique<SetMovementPair>(pair);
    };

    functionMap["flippermove_setDistance"] = [&globalMap](const json& json_object, const string& name) {
        string unit = json_object[name]["fields"]["UNIT"][0];
        Block* distance;
        if(json_object[name]["inputs"]["DISTANCE"][0] == 1){
            distance = new BlankBlockDouble(stod(json_object[name]["inputs"]["DISTANCE"][1][1].get<string>()));
        } else {
            string distance_name = json_object[name]["inputs"]["DISTANCE"][1];
            distance = globalMap[json_object[distance_name]["opcode"]](json_object, distance_name).release();
        }
        return make_unique<SetMotorRotation>(unit, distance);
    };

    return functionMap;
}

#endif