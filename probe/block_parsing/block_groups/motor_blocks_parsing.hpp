#ifndef MOTOR_BLOCKS_PARSING_HPP
#define MOTOR_BLOCKS_PARSING_HPP

#include <map>
#include <functional>
#include <memory>
#include <string>

#include "../../blocks/Block.hpp"
#include "../../blocks/BlockSequence.hpp"
#include "../../external/json.hpp"
#include "../../blocks/motor_blocks/motor_blocks_include.hpp"
#include "../../blocks/blank_blocks/blank_blocks_include.hpp"

using namespace std;
using json = nlohmann::json;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;

FunctionMap createMotorFunctionMap(FunctionMap& globalMap) {
    FunctionMap functionMap;

    //Motor blocks
    functionMap["flippermotor_motorTurnForDirection"] = [&globalMap](const json& json_object, const string& name) {
        bool forward;
        int args = json_object[name]["inputs"]["DIRECTION"][0];
        if(args != 1){
            forward = true;
        }
        else{
            string direction_name = json_object[name]["inputs"]["DIRECTION"][1];
            string fwd = json_object[direction_name]["fields"]["field_flippermotor_custom-icon-direction"][0];
            
            if(fwd == "counterclockwise") {
                forward = true;
            } else {
                forward = false;
            }
        }
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();
        
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            value = globalMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        return make_unique<MotorTurnForDirection>(port, value, forward, unit);
    };

    functionMap["flippermotor_motorGoDirectionToPosition"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        string direction = json_object[name]["fields"]["DIRECTION"][0].get<string>();
        
        string position_name = json_object[name]["inputs"]["POSITION"][1];
        Block* position = globalMap[json_object[position_name]["opcode"]](json_object, position_name).release();
        return make_unique<MotorGoDirectionToPosition>(port, direction, position);
    };

    functionMap ["flippermotor_motorStartDirection"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        bool forward;
        int args = json_object[name]["inputs"]["DIRECTION"][0];
        if(args != 1){
            forward = true;
        }
        else{
            string direction_name = json_object[name]["inputs"]["DIRECTION"][1];
            string fwd = json_object[direction_name]["fields"]["field_flippermotor_custom-icon-direction"][0];
            
            if(fwd == "counterclockwise") {
                forward = true;
            } else {
                forward = false;
            }
        }
        
        return make_unique<MotorStartDirection>(port, forward);
    };

    functionMap ["flippermotor_motorStop"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        return make_unique<MotorStop>(port);
    };

    functionMap ["flippermotor_motorSetSpeed"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        Block* speed;
        if(json_object[name]["inputs"]["SPEED"][0] == 1){
            speed = new BlankBlockDouble(stod(json_object[name]["inputs"]["SPEED"][1][1].get<string>()));
        } else {
            string speed_name = json_object[name]["inputs"]["SPEED"][1];
            speed = globalMap[json_object[speed_name]["opcode"]](json_object, speed_name).release();
        }
        return make_unique<MotorSetSpeed>(port, speed);
    };

    functionMap ["flippermotor_absolutePosition"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        return make_unique<MotorPosition>(port);
    };

    functionMap ["flippermotor_speed"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        return make_unique<MotorSpeed>(port);
    };
    //---------------------------------------------

    return functionMap;

}

#endif