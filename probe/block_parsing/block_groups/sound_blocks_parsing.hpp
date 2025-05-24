#ifndef SOUND_BLOCKS_PARSING_HPP
#define SOUND_BLOCKS_PARSING_HPP

#include <map>
#include <functional>
#include <memory>
#include <string>

#include "../../all_blocks/Block.hpp"
#include "../../all_blocks/BlockSequence.hpp"
#include "../../json.hpp"
#include "../../all_blocks/sound_blocks/sound_blocks_include.hpp"
#include "../../all_blocks/blank_blocks/blank_blocks_include.hpp"

using namespace std;
using json = nlohmann::json;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;

FunctionMap createSoundFunctionMap(FunctionMap& globalMap) {
    FunctionMap functionMap;

    // Sound blocks
    functionMap["flippersound_playSoundUntilDone"] = [&globalMap](const json& json_object, const string& name) {
        /*string sound_name = json_object[name]["inputs"]["SOUND"][1];
        string sound_string = json_object[sound_name]["fields"]["field_flippersound_sound-selector"][0].get<string>();
        json sound_json = json::parse(sound_string);
        string sound = sound_json["name"].get<string>();
        string sound_location = sound_json["location"].get<string>();*/

        string sound_name = json_object[name]["inputs"]["SOUND"][1];
        Block* sound = globalMap[json_object[sound_name]["opcode"]](json_object, sound_name).release();

        return make_unique<PlayUntilDone>(sound);
    };

    functionMap["flippersound_playSound"] = [&globalMap](const json& json_object, const string& name) {
        /*string sound_name_name = json_object[name]["inputs"]["SOUND"][1];
        string sound_string = json_object[sound_name_name]["fields"]["field_flippersound_sound-selector"][0].get<string>();
        json sound_json = json::parse(sound_string);
        string sound_name = sound_json["name"].get<string>();
        string sound_location = sound_json["location"].get<string>();*/

        string sound_name = json_object[name]["inputs"]["SOUND"][1];
        Block* sound = globalMap[json_object[sound_name]["opcode"]](json_object, sound_name).release();

        return make_unique<Play>(sound);
    };

    functionMap["flippersound_beepForTime"] = [&globalMap](const json& json_object, const string& name) {
        Block* time;
        if(json_object[name]["inputs"]["DURATION"][0] == 1){
            time = new BlankBlockDouble(stod(json_object[name]["inputs"]["DURATION"][1][1].get<string>()));
        } else {
            string time_name = json_object[name]["inputs"]["DURATION"][1];
            time = globalMap[json_object[time_name]["opcode"]](json_object, time_name).release();
        }
        string frequency_name = json_object[name]["inputs"]["NOTE"][1];
        Block* frequency = globalMap[json_object[frequency_name]["opcode"]](json_object, frequency_name).release();
        return make_unique<BeepForTime>(time, frequency);
    };

    functionMap["flippersound_beep"] = [&globalMap](const json& json_object, const string& name) {
        string frequency_name = json_object[name]["inputs"]["NOTE"][1];
        Block* frequency = globalMap[json_object[frequency_name]["opcode"]](json_object, frequency_name).release();
        return make_unique<Beep>(frequency);
    };

    functionMap["flippersound_stopSound"] = [&globalMap](const json& json_object, const string& name) {
        return make_unique<StopSound>();
    };

    functionMap["sound_changeeffectby"] = [&globalMap](const json& json_object, const string& name) {
        string effect = json_object[name]["fields"]["EFFECT"][0].get<string>();
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value_name = json_object[name]["inputs"]["VALUE"][1];
            value = globalMap[json_object[value_name]["opcode"]](json_object, value_name).release();
        }
        return make_unique<ChangeEffectBy>(effect, value);
    };

    functionMap["sound_seteffectto"] = [&globalMap](const json& json_object, const string& name) {
        string effect = json_object[name]["fields"]["EFFECT"][0].get<string>();
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value_name = json_object[name]["inputs"]["VALUE"][1];
            value = globalMap[json_object[value_name]["opcode"]](json_object, value_name).release();
        }
        return make_unique<SetEffectTo>(effect, value);
    };

    functionMap["sound_cleareffects"] = [&globalMap](const json& json_object, const string& name) {
        return make_unique<ClearEffects>();
    };

    functionMap["sound_changevolumeby"] = [&globalMap](const json& json_object, const string& name) {
        Block* volume;
        if(json_object[name]["inputs"]["VOLUME"][0] == 1){
            volume = new BlankBlockDouble(stod(json_object[name]["inputs"]["VOLUME"][1][1].get<string>()));
        } else {
            string volume_name = json_object[name]["inputs"]["VOLUME"][1];
            volume = globalMap[json_object[volume_name]["opcode"]](json_object, volume_name).release();
        }
        return make_unique<ChangeVolumeBy>(volume);
    };

    functionMap["sound_setvolumeto"] = [&globalMap](const json& json_object, const string& name) {
        Block* volume;
        if(json_object[name]["inputs"]["VOLUME"][0] == 1){
            volume = new BlankBlockDouble(stod(json_object[name]["inputs"]["VOLUME"][1][1].get<string>()));
        } else {
            string volume_name = json_object[name]["inputs"]["VOLUME"][1];
            volume = globalMap[json_object[volume_name]["opcode"]](json_object, volume_name).release();
        }
        return make_unique<SetVolumeTo>(volume);
    };

    functionMap ["sound_volume"] = [&globalMap](const json& json_object, const string& name) {
        return make_unique<Volume>();
    };
    //--------------------------------------------

    return functionMap;

}

#endif