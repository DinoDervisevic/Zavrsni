#ifndef SENSOR_BLOCKS_INCLUDE_HPP
#define SENSOR_BLOCKS_INCLUDE_HPP

#include <map>
#include <functional>
#include <memory>
#include <string>

#include "../../blocks/Block.hpp"
#include "../../blocks/BlockSequence.hpp"
#include "../../external/json.hpp"
#include "../../blocks/sensor_blocks/sensor_blocks_include.hpp"
#include "../../blocks/blank_blocks/blank_blocks_include.hpp"

using namespace std;
using json = nlohmann::json;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;
 
FunctionMap createSensorFunctionMap(FunctionMap& globalMap) {
    FunctionMap functionMap;

    // Sensor blocks
    functionMap ["flippersensors_isColor"] = [&globalMap](const json& json_object, const string& name) {
        string color_name = json_object[name]["inputs"]["VALUE"][1];
        Block* color = globalMap[json_object[color_name]["opcode"]](json_object, color_name).release();
        
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        return make_unique<IsColor>(port, color);
    };

    functionMap ["flippersensors_color"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        return make_unique<SensorColor>(port);
    };

    functionMap ["flippersensors_isReflectivity"] = [&globalMap](const json& json_object, const string& name) {
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["VALUE"][1];
            value = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        string comparator = json_object[name]["fields"]["COMPARATOR"][0].get<string>();

        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        return make_unique<IsReflectivity>(port, comparator, value);
    };

    functionMap ["flippersensors_reflectivity"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        return make_unique<SensorReflectivity>(port);
    };

    functionMap ["flippersensors_isPressed"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string pressed = json_object[name]["fields"]["PRESSED"][0].get<string>();

        return make_unique<IsPressed>(port, pressed);
    }; 

    functionMap ["flippersensors_force"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();

        return make_unique<SensorForce>(port, unit);
    };

    functionMap ["flippersensors_isDistance"] = [&globalMap](const json& json_object, const string& name) {
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value_name = json_object[name]["inputs"]["VALUE"][1];
            value = globalMap[json_object[value_name]["opcode"]](json_object, value_name).release();
        }

        string comparator = json_object[name]["fields"]["COMPARATOR"][0].get<string>();

        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();

        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        return make_unique<IsDistance>(port, comparator, value, unit);
    };

    functionMap ["flippersensors_distance"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();

        return make_unique<SensorDistance>(port, unit);
    };

    functionMap ["flippersensors_isTilted"] = [&globalMap](const json& json_object, const string& name) {
        string angle_name = json_object[name]["inputs"]["VALUE"][1];
        Block* angle = globalMap[json_object[angle_name]["opcode"]](json_object, angle_name).release();
        
        return make_unique<IsTilted>(angle);
    };

    functionMap ["flippersensors_isorientation"] = [&globalMap](const json& json_object, const string& name) {
        string orientation = json_object[name]["fields"]["ORIENTATION"][0].get<string>();
        return make_unique<IsOrientation>(orientation);
    };

    functionMap ["flippersensors_ismotion"] = [&globalMap](const json& json_object, const string& name) {
        string motion = json_object[name]["fields"]["MOTION"][0].get<string>();
        return make_unique<IsMotion>(motion);
    };

    functionMap ["flippersensors_orientationAxis"] = [&globalMap](const json& json_object, const string& name) {
        string axis = json_object[name]["fields"]["AXIS"][0].get<string>();
        return make_unique<OrientationAxis>(axis);
    };

    functionMap ["flippersensors_resetYaw"] = [&globalMap](const json& json_object, const string& name) {
        return make_unique<ResetYawAngle>();
    };

    functionMap ["flippersensors_buttonIsPressed"] = [&globalMap](const json& json_object, const string& name) {
        string button = json_object[name]["fields"]["BUTTON"][0].get<string>();
        string event = json_object[name]["fields"]["EVENT"][0].get<string>();
        return make_unique<IsButtonPressed>(button, event);
    };

    functionMap ["flippersensors_timer"] = [&globalMap](const json& json_object, const string& name) {
        return make_unique<Timer>();
    };

    functionMap ["flippersensors_resetTimer"] = [&globalMap](const json& json_object, const string& name) {
        return make_unique<ResetTimer>();
    };

    //---------------------------------------------


    return functionMap;

}


#endif