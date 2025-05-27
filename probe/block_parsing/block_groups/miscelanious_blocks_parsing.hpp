#ifndef MISCELANIOUS_BLOCKS_PARSING_HPP
#define MISCELANIOUS_BLOCKS_PARSING_HPP

#include <map>
#include <functional>
#include <memory>
#include <string>

#include "../../blocks/Block.hpp"
#include "../../blocks/BlockSequence.hpp"
#include "../../external/json.hpp"
#include "../../blocks/blank_blocks/blank_blocks_include.hpp"

using namespace std;
using json = nlohmann::json;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;
 
FunctionMap createMiscelaniousFunctionMap(FunctionMap& globalMap) {
    FunctionMap functionMap;

     // Miscelanious helper "blocks"
    functionMap["flippermove_movement-port-selector"] = [&globalMap](const json& json_object, const string& name) {
        string port = json_object[name]["fields"]["field_flippermove_multiple-port-selector"][0].get<string>();
        return make_unique<BlankBlockString>(port);
    };

    functionMap ["flippermotor_multiple-port-selector"] = [&globalMap](const json& json_object, const string& name) {
        string port = json_object[name]["fields"]["field_flippermotor_multiple-port-selector"][0].get<string>();
        return make_unique<BlankBlockString>(port);
    };

    functionMap["flippermotor_custom-angle"] = [&globalMap](const json& json_object, const string& name) {
        int angle = json_object[name]["fields"]["field_flippermotor_custom-angle"][0].get<int>();
        return make_unique<BlankBlockInt>(angle);
    };

    functionMap ["flipperevents_color-selector"] = [&globalMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flipperevents_color-selector"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(color));
    };

    functionMap ["flipperevents_color-sensor-selector"] = [&globalMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flipperevents_color-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(color);
    };

    functionMap ["flipperevents_force-sensor-selector"] = [&globalMap](const json& json_object, const string& name) {
        string force = json_object[name]["fields"]["field_flipperevents_force-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(force);
    };

    functionMap ["flipperevents_distance-sensor-selector"] = [&globalMap](const json& json_object, const string& name) {
        string distance = json_object[name]["fields"]["field_flipperevents_distance-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(distance);
    };

    functionMap ["flipperevents_custom-tilted"] = [&globalMap](const json& json_object, const string& name) {
        string tilt = json_object[name]["fields"]["field_flipperevents_custom-tilted"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(tilt));
    };

    functionMap ["flippersensors_color-selector"] = [&globalMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flippersensors_color-selector"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(color));
    };

    functionMap ["flippersensors_color-sensor-selector"] = [&globalMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flippersensors_color-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(color);
    };

    functionMap ["flippersensors_force-sensor-selector"] = [&globalMap](const json& json_object, const string& name) {
        string force = json_object[name]["fields"]["field_flippersensors_force-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(force);
    };

    functionMap ["flippersensors_distance-sensor-selector"] = [&globalMap](const json& json_object, const string& name) {
        string distance = json_object[name]["fields"]["field_flippersensors_distance-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(distance);
    };

    functionMap ["flippersensors_custom-tilted"] = [&globalMap](const json& json_object, const string& name) {
        string tilt = json_object[name]["fields"]["field_flippersensors_custom-tilted"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(tilt));
    };

    functionMap["flipperlight_matrix-5x5-brightness-image"] = [&globalMap](const json& json_object, const string& name) {
        string image = json_object[name]["fields"]["field_flipperlight_matrix-5x5-brightness-image"][0].get<string>();
        return make_unique<BlankBlockString>(image);
    };

    functionMap["flipperlight_menu_orientation"] = [&globalMap](const json& json_object, const string& name) {
        string orientation = json_object[name]["fields"]["orientation"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(orientation));
    };

    functionMap["flipperlight_color-selector-vertical"] = [&globalMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flipperlight_color-selector-vertical"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(color));
    };

    functionMap["flippersound_custom-piano"] = [&globalMap](const json& json_object, const string& name) {
        //Scratch can send either an integer or a string here
        auto& note_json = json_object[name]["fields"]["field_flippersound_custom-piano"][0];
        int note = 0;
        if (note_json.is_string()) {
            note = stoi(note_json.get<string>());
        } else if (note_json.is_number_integer()) {
            note = note_json.get<int>();
        } else {
            cerr << "Unknown type for piano note!" << endl;
        }
        return make_unique<BlankBlockInt>(note);
    };

    functionMap["flipperlight_matrix-pixel-index"] = [&globalMap](const json& json_object, const string& name) {
        string index = json_object[name]["fields"]["field_flipperlight_matrix-pixel-index"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(index));
    };

    functionMap["flipperlight_distance-sensor-selector"] = [&globalMap](const json& json_object, const string& name) {
        string distance = json_object[name]["fields"]["field_flipperlight_distance-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(distance);
    };

    functionMap["flipperlight_led-selector"] = [&globalMap](const json& json_object, const string& name) {
        string led = json_object[name]["fields"]["field_flipperlight_led-selector"][0].get<string>();
        return make_unique<BlankBlockString>(led);
    };

    functionMap["flippersound_sound-selector"] = [&globalMap](const json& json_object, const string& name) {
        string sound = json_object[name]["fields"]["field_flippersound_sound-selector"][0].get<string>();
        return make_unique<BlankBlockString>(sound);
    };


    return functionMap;

}



#endif