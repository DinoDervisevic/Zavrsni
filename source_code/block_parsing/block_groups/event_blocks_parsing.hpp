#ifndef EVENT_BLOCKS_PARSING_HPP
#define EVENT_BLOCKS_PARSING_HPP

#include <map>
#include <functional>
#include <memory>
#include <string>

#include "../../blocks/Block.hpp"
#include "../../blocks/BlockSequence.hpp"
#include "../../external/json.hpp"
#include "../../blocks/event_blocks/event_blocks_include.hpp"
#include "../../blocks/blank_blocks/blank_blocks_include.hpp"

using namespace std;
using json = nlohmann::json;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;

FunctionMap createEventFunctionMap(FunctionMap& globalMap) {
    FunctionMap functionMap;

    // Event blocks
    functionMap["flipperevents_whenProgramStarts"] = [&globalMap](const json& json_object, const string& name) {
        return make_unique<WhenProgramStarts>();
    };

    functionMap ["flipperevents_whenColor"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string color_name = json_object[name]["inputs"]["OPTION"][1];
        Block* color = globalMap[json_object[color_name]["opcode"]](json_object, color_name).release();
        
        return make_unique<WhenColor>(port, color);
    };

    functionMap ["flipperevents_whenPressed"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string event = json_object[name]["fields"]["OPTION"][0];
        return make_unique<WhenPressed>(port, event);
    };

    functionMap ["flipperevents_whenDistance"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string option = json_object[name]["fields"]["COMPARATOR"][0];
        
        Block* distance;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            if (json_object[name]["inputs"]["VALUE"][1][1].get<string>() == ""){
                distance = new BlankBlockDouble(0.0);
            }
            else distance = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            distance = globalMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }

        string unit = json_object[name]["fields"]["UNIT"][0];
        
        return make_unique<WhenDistance>(port, option, distance, unit);
    };

    functionMap ["flipperevents_whenTilted"] = [&globalMap](const json& json_object, const string& name) {
        string angle_name = json_object[name]["inputs"]["VALUE"][1];
        Block* angle = globalMap[json_object[angle_name]["opcode"]](json_object, angle_name).release();
        
        return make_unique<WhenTilted>(angle);
    };

    functionMap ["flipperevents_whenOrientation"] = [&globalMap](const json& json_object, const string& name) {
        string orientation = json_object[name]["fields"]["VALUE"][0];
        
        return make_unique<WhenOrientation>(orientation);
    }; 

    functionMap ["flipperevents_whenGesture"] = [&globalMap](const json& json_object, const string& name) {
        string gesture = json_object[name]["fields"]["EVENT"][0];
        
        return make_unique<WhenGesture>(gesture);
    };

    functionMap ["flipperevents_whenButton"] = [&globalMap](const json& json_object, const string& name) {
        string button = json_object[name]["fields"]["BUTTON"][0];
        string event = json_object[name]["fields"]["EVENT"][0];
        
        return make_unique<WhenButton>(button, event);
    };

    functionMap ["flipperevents_whenTimer"] = [&globalMap](const json& json_object, const string& name) {
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            if (json_object[name]["inputs"]["VALUE"][1][1].get<string>() == ""){
                value = new BlankBlockDouble(0.0);
            }
            else value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            value = globalMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        
        return make_unique<WhenTimer>(value);
    };

    functionMap["flipperevents_whenCondition"] = [&globalMap](const json& json_object, const string& name) {
        Block* condition;
        string from_name = json_object[name]["inputs"]["CONDITION"][1];
        condition = globalMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        
        return make_unique<WhenCondition>(condition);
    };

    functionMap["event_whenbroadcastreceived"] = [&globalMap](const json& json_object, const string& name) {
        string broadcast = json_object[name]["fields"]["BROADCAST_OPTION"][0];
        return make_unique<WhenBroadcastReceived>(broadcast);
    };

    functionMap ["event_broadcast"] = [&globalMap](const json& json_object, const string& name) {
        Block* broadcast;
        if(json_object[name]["inputs"]["BROADCAST_INPUT"][0] == 1){
            broadcast = new BlankBlockString(json_object[name]["inputs"]["BROADCAST_INPUT"][1][1].get<string>());
        } else {
            string from_name = json_object[name]["inputs"]["BROADCAST_INPUT"][1];
            broadcast = globalMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        return make_unique<Broadcast>(broadcast);
    };

    functionMap ["event_broadcastandwait"] = [&globalMap](const json& json_object, const string& name) {
        Block* broadcast;
        if(json_object[name]["inputs"]["BROADCAST_INPUT"][0] == 1){
            broadcast = new BlankBlockString(json_object[name]["inputs"]["BROADCAST_INPUT"][1][1].get<string>());
        } else {
            string from_name = json_object[name]["inputs"]["BROADCAST_INPUT"][1];
            broadcast = globalMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        return make_unique<BroadcastAndWait>(broadcast);
    };
    //--------------------------------------------

    return functionMap;

}

#endif