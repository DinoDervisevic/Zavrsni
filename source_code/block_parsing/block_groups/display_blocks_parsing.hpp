#ifndef DISPLAY_BLOCKS_PARSING_HPP
#define DISPLAY_BLOCKS_PARSING_HPP

#include <map>
#include <functional>
#include <memory>
#include <string>

#include "../../blocks/Block.hpp"
#include "../../blocks/BlockSequence.hpp"
#include "../../external/json.hpp"
#include "../../blocks/display_blocks/display_blocks_include.hpp"
#include "../../blocks/blank_blocks/blank_blocks_include.hpp"

using namespace std;
using json = nlohmann::json;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;

FunctionMap createDisplayFunctionMap(FunctionMap& globalMap) {
    FunctionMap functionMap;

     // Display blocks
    functionMap["flipperlight_lightDisplayImageOnForTime"] = [&globalMap](const json& json_object, const string& name) {
        string matrix_name = json_object[name]["inputs"]["MATRIX"][1];
        Block* image = globalMap[json_object[matrix_name]["opcode"]](json_object, matrix_name).release();
        double time = stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>());
        return make_unique<DisplayImageForTime>(image, time);
    };

    functionMap["flipperlight_lightDisplayImageOn"] = [&globalMap](const json& json_object, const string& name) {
        string matrix_name = json_object[name]["inputs"]["MATRIX"][1];
        Block* image = globalMap[json_object[matrix_name]["opcode"]](json_object, matrix_name).release();
        return make_unique<DisplayImage>(image);
    };

    functionMap["flipperlight_lightDisplayText"] = [&globalMap](const json& json_object, const string& name) {
        Block* text;
        if(json_object[name]["inputs"]["TEXT"][0] == 1){
            text = new BlankBlockString(json_object[name]["inputs"]["TEXT"][1][1].get<string>());
        } else {
            string text_name = json_object[name]["inputs"]["TEXT"][1];
            text = globalMap[json_object[text_name]["opcode"]](json_object, text_name).release();
        }
        return make_unique<DisplayText>(text);
    };

    functionMap["flipperlight_lightDisplayOff"] = [&globalMap](const json& json_object, const string& name) {
        return make_unique<DisplayOff>();
    };

    functionMap["flipperlight_lightDisplaySetBrightness"] = [&globalMap](const json& json_object, const string& name) {
        Block*brightness;
        if(json_object[name]["inputs"]["BRIGHTNESS"][0] == 1){
            brightness = new BlankBlockDouble(stod(json_object[name]["inputs"]["BRIGHTNESS"][1][1].get<string>()));
        } else {
            string brightness_name = json_object[name]["inputs"]["BRIGHTNESS"][1];
            brightness = globalMap[json_object[brightness_name]["opcode"]](json_object, brightness_name).release();
        }
        return make_unique<SetPixelbrightness>(brightness);
    };

    functionMap["flipperlight_lightDisplaySetPixel"] = [&globalMap](const json& json_object, const string& name) {
        string x_name = json_object[name]["inputs"]["X"][1];
        string y_name = json_object[name]["inputs"]["Y"][1];
        Block* x = globalMap[json_object[x_name]["opcode"]](json_object, x_name).release();
        Block* y = globalMap[json_object[y_name]["opcode"]](json_object, y_name).release();
        
        Block* brightness;
        if(json_object[name]["inputs"]["BRIGHTNESS"][0] == 1){
            brightness = new BlankBlockDouble(stod(json_object[name]["inputs"]["BRIGHTNESS"][1][1].get<string>()));
        } else {
            string distance_name = json_object[name]["inputs"]["BRIGHTNESS"][1];
            brightness = globalMap[json_object[distance_name]["opcode"]](json_object, distance_name).release();
        }

        return make_unique<SetPixel>(x, y, brightness);
    };

    functionMap["flipperlight_lightDisplayRotate"] = [&globalMap](const json& json_object, const string& name) {
        bool forward;
        int args = json_object[name]["inputs"]["DIRECTION"][0];
        if(args != 1){
            forward = true;
        }
        else{
            string direction_name = json_object[name]["inputs"]["DIRECTION"][1];
            string fwd = json_object[direction_name]["fields"]["field_flipperlight_custom-icon-direction"][0];
            
            if(fwd == "counterclockwise") {
                forward = true;
            } else {
                forward = false;
            }
        }
        return make_unique<DisplayRotate>(forward);
    };

    functionMap["flipperlight_lightDisplaySetOrientation"] = [&globalMap](const json& json_object, const string& name) {
        string orientation_name = json_object[name]["inputs"]["ORIENTATION"][1];
        Block* orientation = globalMap[json_object[orientation_name]["opcode"]](json_object, orientation_name).release();
        return make_unique<DisplaySetorientation>(orientation);
    };

    functionMap["flipperlight_centerButtonLight"] = [&globalMap](const json& json_object, const string& name) {
        string color_name = json_object[name]["inputs"]["COLOR"][1];
        Block* color = globalMap[json_object[color_name]["opcode"]](json_object, color_name).release();
        return make_unique<CenterButtonLight>(color);
    };

    functionMap["flipperlight_ultrasonicLightUp"] = [&globalMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        string value_name = json_object[name]["inputs"]["VALUE"][1];
        Block* value = globalMap[json_object[value_name]["opcode"]](json_object, value_name).release();
        Block* port = globalMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        return make_unique<UltrasonicSensorLight>(value, port);
    };
    //--------------------------------------------

    return functionMap;

}

#endif