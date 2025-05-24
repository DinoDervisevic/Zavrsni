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

#include "json.hpp"
#include "robot/robot.hpp"
#include "functions.hpp"
#include "display_letters.hpp"
#include "all_blocks/common_block_includes.hpp"
#include "all_blocks/all_blocks_include.hpp"

using namespace std;

using json = nlohmann::json;

BlockSequence* processBlock(const json& blocks, string key);



//-----------FUNCTION MAP---------------------

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;

FunctionMap createFunctionMap() {
    FunctionMap functionMap;

    // Event blocks
    functionMap["flipperevents_whenProgramStarts"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<WhenProgramStarts>();
    };

    functionMap ["flipperevents_whenColor"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string color_name = json_object[name]["inputs"]["OPTION"][1];
        Block* color = functionMap[json_object[color_name]["opcode"]](json_object, color_name).release();
        
        return make_unique<WhenColor>(port, color);
    };

    functionMap ["flipperevents_whenPressed"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string event = json_object[name]["fields"]["OPTION"][0];

        return make_unique<WhenPressed>(port, event);
    };

    functionMap ["flipperevents_whenDistance"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string option = json_object[name]["inputs"]["COMPARATOR"][0];
        
        Block* distance;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            distance = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            distance = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }

        string unit = json_object[name]["fields"]["UNIT"][0];
        
        return make_unique<WhenDistance>(port, option, distance, unit);
    };

    functionMap ["flipperevents_whenTilted"] = [&functionMap](const json& json_object, const string& name) {
        string angle_name = json_object[name]["inputs"]["VALUE"][1];
        Block* angle = functionMap[json_object[angle_name]["opcode"]](json_object, angle_name).release();
        
        return make_unique<WhenTilted>(angle);
    };

    functionMap ["flipperevents_whenOrientation"] = [&functionMap](const json& json_object, const string& name) {
        string orientation = json_object[name]["fields"]["VALUE"][0];
        
        return make_unique<WhenOrientation>(orientation);
    }; 

    functionMap ["flipperevents_whenGesture"] = [&functionMap](const json& json_object, const string& name) {
        string gesture = json_object[name]["fields"]["EVENT"][0];
        
        return make_unique<WhenGesture>(gesture);
    };

    functionMap ["flipperevents_whenButton"] = [&functionMap](const json& json_object, const string& name) {
        string button = json_object[name]["fields"]["BUTTON"][0];
        string event = json_object[name]["fields"]["EVENT"][0];
        
        return make_unique<WhenButton>(button, event);
    };

    functionMap ["flipperevents_whenTimer"] = [&functionMap](const json& json_object, const string& name) {
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        
        return make_unique<WhenTimer>(value);
    };

    functionMap["flipperevents_whenCondition"] = [&functionMap](const json& json_object, const string& name) {
        Block* condition;
        string from_name = json_object[name]["inputs"]["CONDITION"][1];
        condition = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        
        return make_unique<WhenCondition>(condition);
    };

    functionMap["event_whenbroadcastreceived"] = [&functionMap](const json& json_object, const string& name) {
        string broadcast = json_object[name]["fields"]["BROADCAST_OPTION"][0];
        return make_unique<WhenBroadcastReceived>(broadcast);
    };

    functionMap ["event_broadcast"] = [&functionMap](const json& json_object, const string& name) {
        Block* broadcast;
        if(json_object[name]["inputs"]["BROADCAST_INPUT"][0] == 1){
            broadcast = new BlankBlockString(json_object[name]["inputs"]["BROADCAST_INPUT"][1][1].get<string>());
        } else {
            string from_name = json_object[name]["inputs"]["BROADCAST_INPUT"][1];
            broadcast = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        return make_unique<Broadcast>(broadcast);
    };

    functionMap ["event_broadcastandwait"] = [&functionMap](const json& json_object, const string& name) {
        Block* broadcast;
        if(json_object[name]["inputs"]["BROADCAST_INPUT"][0] == 1){
            broadcast = new BlankBlockString(json_object[name]["inputs"]["BROADCAST_INPUT"][1][1].get<string>());
        } else {
            string from_name = json_object[name]["inputs"]["BROADCAST_INPUT"][1];
            broadcast = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        return make_unique<BroadcastAndWait>(broadcast);
    };
    //--------------------------------------------

    // Operator blocks
    functionMap["operator_random"] = [&functionMap](const json& json_object, const string& name) {
        Block* from;
        Block* to;
        if(json_object[name]["inputs"]["FROM"][0] == 1){
            from = new BlankBlockInt(stoi(json_object[name]["inputs"]["FROM"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["FROM"][1];
            from = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }

        if(json_object[name]["inputs"]["TO"][0] == 1){
            to = new BlankBlockInt(stoi(json_object[name]["inputs"]["TO"][1][1].get<string>()));
        } else {
            string to_name = json_object[name]["inputs"]["TO"][1];
            to = functionMap[json_object[to_name]["opcode"]](json_object, to_name).release();
        }
        return make_unique<PickRandom>(from, to);
    };

    functionMap["operator_add"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Add>(value1, value2);
    };

    functionMap["operator_subtract"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Subtract>(value1, value2);
    };

    functionMap["operator_multiply"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Multiply>(value1, value2);
    };

    functionMap["operator_divide"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Divide>(value1, value2);
    };

    functionMap["operator_lt"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["OPERAND1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["OPERAND1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["OPERAND2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["OPERAND2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<LessThan>(value1, value2);
    };

    functionMap["operator_equals"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["OPERAND1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["OPERAND1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["OPERAND2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["OPERAND2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Equals>(value1, value2);
    };

    functionMap["operator_gt"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["OPERAND1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["OPERAND1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["OPERAND2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["OPERAND2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<GreaterThan>(value1, value2);
    };

    functionMap["operator_and"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if (json_object[name]["inputs"].contains("OPERAND1")) {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        } else {
            cerr << "Error: Operator AND is missing input one!" << endl;
        }

        if (json_object[name]["inputs"].contains("OPERAND2")) {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        } else {
            cerr << "Error: Operator AND is missing input two!" << endl;
        }
        return make_unique<And>(value1, value2);
    };

    functionMap["operator_or"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if (json_object[name]["inputs"].contains("OPERAND1")) {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        } else {
            cerr << "Error: Operator OR is missing input one!" << endl;
        }

        if (json_object[name]["inputs"].contains("OPERAND2")) {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        } else {
            cerr << "Error: Operator OR is missing input two!" << endl;
        }
        return make_unique<Or>(value1, value2);
    };

    functionMap["operator_not"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        if (json_object[name]["inputs"].contains("OPERAND")) {
            string value1_name = json_object[name]["inputs"]["OPERAND"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        } else {
            cerr << "Error: Operator NOT is missing input!" << endl;
        }
        return make_unique<Not>(value1);
    };

    functionMap["flipperoperator_isInBetween"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        Block* value3;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["VALUE"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["LOW"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["LOW"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["LOW"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }

        if(json_object[name]["inputs"]["HIGH"][0] == 1){
            value3 = new BlankBlockDouble(stod(json_object[name]["inputs"]["HIGH"][1][1].get<string>()));
        } else {
            string value3_name = json_object[name]["inputs"]["HIGH"][1];
            value3 = functionMap[json_object[value3_name]["opcode"]](json_object, value3_name).release();
        }
        return make_unique<IsInBetween>(value1, value2, value3);
    };

    functionMap["operator_join"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["STRING1"][0] == 1){
            value1 = new BlankBlockString(json_object[name]["inputs"]["STRING1"][1][1].get<string>());
        } else {
            string value1_name = json_object[name]["inputs"]["STRING1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["STRING2"][0] == 1){
            value2 = new BlankBlockString(json_object[name]["inputs"]["STRING2"][1][1].get<string>());
        } else {
            string value2_name = json_object[name]["inputs"]["STRING2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Join>(value1, value2);
    };

    functionMap["operator_letterof"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["STRING"][0] == 1){
            value2 = new BlankBlockString(json_object[name]["inputs"]["STRING"][1][1].get<string>());
        } else {
            string value1_name = json_object[name]["inputs"]["STRING"][1];
            value2 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["LETTER"][0] == 1){
            value1 = new BlankBlockInt(stoi(json_object[name]["inputs"]["LETTER"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["LETTER"][1];
            value1 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<LetterOf>(value1, value2);
    };

    functionMap["operator_length"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        if(json_object[name]["inputs"]["STRING"][0] == 1){
            value1 = new BlankBlockString(json_object[name]["inputs"]["STRING"][1][1].get<string>());
        } else {
            string value1_name = json_object[name]["inputs"]["STRING"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }
        return make_unique<LengthOf>(value1);
    };

    functionMap["operator_contains"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["STRING1"][0] == 1){
            value1 = new BlankBlockString(json_object[name]["inputs"]["STRING1"][1][1].get<string>());
        } else {
            string value1_name = json_object[name]["inputs"]["STRING1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["STRING2"][0] == 1){
            value2 = new BlankBlockString(json_object[name]["inputs"]["STRING2"][1][1].get<string>());
        } else {
            string value2_name = json_object[name]["inputs"]["STRING2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Contains>(value1, value2);
    };

    functionMap["operator_mod"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Modulus>(value1, value2);
    };

    functionMap["operator_round"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        if(json_object[name]["inputs"]["NUM"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }
        return make_unique<Round>(value1);
    };

    functionMap["operator_mathop"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        if(json_object[name]["inputs"]["NUM"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }
        return make_unique<MathOp>(value1, json_object[name]["fields"]["OPERATOR"][0]);
    };
    //--------------------------------------------

    // Movement blocks
    functionMap["flippermove_move"] = [&functionMap](const json& json_object, const string& name) {
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
            value = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }

        return make_unique<Move>(forward, value, unit);
    };

    functionMap["flippermove_startMove"] = [&functionMap](const json& json_object, const string& name) {
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

    functionMap["flippermove_rotation-wheel"] = [&functionMap](const json& json_object, const string& name) {
        string direction = json_object[name]["fields"]["field_flippermove_rotation-wheel"][0];
        int value = stoi(direction.substr(direction.find(" ") + 1));
        return make_unique<BlankBlockInt>(value);
    };

    functionMap["flippermove_steer"] = [&functionMap](const json& json_object, const string& name) {
        string direction_name = json_object[name]["inputs"]["STEERING"][1];
        Block* direction = functionMap[json_object[direction_name]["opcode"]](json_object, direction_name).release();
        
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        
        string unit = json_object[name]["fields"]["UNIT"][0];
        return make_unique<Steer>(direction, value, unit);
    };

    functionMap["flippermove_startSteer"] = [&functionMap](const json& json_object, const string& name) {
        string direction_name = json_object[name]["inputs"]["STEERING"][1];
        Block* direction = functionMap[json_object[direction_name]["opcode"]](json_object, direction_name).release();
        return make_unique<StartSteer>(direction);
    };

    functionMap["flippermove_stopMove"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<StopMoving>();
    };

    functionMap["flippermove_movementSpeed"] = [&functionMap](const json& json_object, const string& name) {
        Block* speed;
        if(json_object[name]["inputs"]["SPEED"][0] == 1){
            speed = new BlankBlockDouble(stod(json_object[name]["inputs"]["SPEED"][1][1].get<string>()));
        } else {
            string speed_name = json_object[name]["inputs"]["SPEED"][1];
            speed = functionMap[json_object[speed_name]["opcode"]](json_object, speed_name).release();
        }
        return make_unique<SetMovementSpeed>(speed);
    };

    functionMap["flippermove_setMovementPair"] = [&functionMap](const json& json_object, const string& name) {
        string pair_name = json_object[name]["inputs"]["PAIR"][1];
        Block* pair = functionMap[json_object[pair_name]["opcode"]](json_object, pair_name).release();
        return make_unique<SetMovementPair>(pair);
    };

    functionMap["flippermove_setDistance"] = [&functionMap](const json& json_object, const string& name) {
        string unit = json_object[name]["fields"]["UNIT"][0];
        Block* distance;
        if(json_object[name]["inputs"]["DISTANCE"][0] == 1){
            distance = new BlankBlockDouble(stod(json_object[name]["inputs"]["DISTANCE"][1][1].get<string>()));
        } else {
            string distance_name = json_object[name]["inputs"]["DISTANCE"][1];
            distance = functionMap[json_object[distance_name]["opcode"]](json_object, distance_name).release();
        }
        return make_unique<SetMotorRotation>(unit, distance);
    };
    //--------------------------------------------
    // Display blocks
    functionMap["flipperlight_lightDisplayImageOnForTime"] = [&functionMap](const json& json_object, const string& name) {
        string matrix_name = json_object[name]["inputs"]["MATRIX"][1];
        Block* image = functionMap[json_object[matrix_name]["opcode"]](json_object, matrix_name).release();
        double time = stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>());
        return make_unique<DisplayImageForTime>(image, time);
    };

    functionMap["flipperlight_lightDisplayImageOn"] = [&functionMap](const json& json_object, const string& name) {
        string matrix_name = json_object[name]["inputs"]["MATRIX"][1];
        Block* image = functionMap[json_object[matrix_name]["opcode"]](json_object, matrix_name).release();
        return make_unique<DisplayImage>(image);
    };

    functionMap["flipperlight_lightDisplayText"] = [&functionMap](const json& json_object, const string& name) {
        Block* text;
        if(json_object[name]["inputs"]["TEXT"][0] == 1){
            text = new BlankBlockString(json_object[name]["inputs"]["TEXT"][1][1].get<string>());
        } else {
            string text_name = json_object[name]["inputs"]["TEXT"][1];
            text = functionMap[json_object[text_name]["opcode"]](json_object, text_name).release();
        }
        return make_unique<DisplayText>(text);
    };

    functionMap["flipperlight_lightDisplayOff"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<DisplayOff>();
    };

    functionMap["flipperlight_lightDisplaySetBrightness"] = [&functionMap](const json& json_object, const string& name) {
        Block*brightness;
        if(json_object[name]["inputs"]["BRIGHTNESS"][0] == 1){
            brightness = new BlankBlockDouble(stod(json_object[name]["inputs"]["BRIGHTNESS"][1][1].get<string>()));
        } else {
            string brightness_name = json_object[name]["inputs"]["BRIGHTNESS"][1];
            brightness = functionMap[json_object[brightness_name]["opcode"]](json_object, brightness_name).release();
        }
        return make_unique<SetPixelbrightness>(brightness);
    };

    functionMap["flipperlight_lightDisplaySetPixel"] = [&functionMap](const json& json_object, const string& name) {
        string x_name = json_object[name]["inputs"]["X"][1];
        string y_name = json_object[name]["inputs"]["Y"][1];
        Block* x = functionMap[json_object[x_name]["opcode"]](json_object, x_name).release();
        Block* y = functionMap[json_object[y_name]["opcode"]](json_object, y_name).release();
        
        Block* brightness;
        if(json_object[name]["inputs"]["BRIGHTNESS"][0] == 1){
            brightness = new BlankBlockDouble(stod(json_object[name]["inputs"]["BRIGHTNESS"][1][1].get<string>()));
        } else {
            string distance_name = json_object[name]["inputs"]["BRIGHTNESS"][1];
            brightness = functionMap[json_object[distance_name]["opcode"]](json_object, distance_name).release();
        }

        return make_unique<SetPixel>(x, y, brightness);
    };

    functionMap["flipperlight_lightDisplayRotate"] = [&functionMap](const json& json_object, const string& name) {
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

    functionMap["flipperlight_lightDisplaySetOrientation"] = [&functionMap](const json& json_object, const string& name) {
        string orientation_name = json_object[name]["inputs"]["ORIENTATION"][1];
        Block* orientation = functionMap[json_object[orientation_name]["opcode"]](json_object, orientation_name).release();
        return make_unique<DisplaySetorientation>(orientation);
    };

    functionMap["flipperlight_centerButtonLight"] = [&functionMap](const json& json_object, const string& name) {
        string color_name = json_object[name]["inputs"]["COLOR"][1];
        Block* color = functionMap[json_object[color_name]["opcode"]](json_object, color_name).release();
        return make_unique<CenterButtonLight>(color);
    };

    functionMap["flipperlight_ultrasonicLightUp"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        string value_name = json_object[name]["inputs"]["VALUE"][1];
        Block* value = functionMap[json_object[value_name]["opcode"]](json_object, value_name).release();
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        return make_unique<UltrasonicSensorLight>(value, port);
    };
    //--------------------------------------------
    // Sound blocks
    functionMap["flippersound_playSoundUntilDone"] = [&functionMap](const json& json_object, const string& name) {
        /*string sound_name = json_object[name]["inputs"]["SOUND"][1];
        string sound_string = json_object[sound_name]["fields"]["field_flippersound_sound-selector"][0].get<string>();
        json sound_json = json::parse(sound_string);
        string sound = sound_json["name"].get<string>();
        string sound_location = sound_json["location"].get<string>();*/

        string sound_name = json_object[name]["inputs"]["SOUND"][1];
        Block* sound = functionMap[json_object[sound_name]["opcode"]](json_object, sound_name).release();

        return make_unique<PlayUntilDone>(sound);
    };

    functionMap["flippersound_playSound"] = [&functionMap](const json& json_object, const string& name) {
        /*string sound_name_name = json_object[name]["inputs"]["SOUND"][1];
        string sound_string = json_object[sound_name_name]["fields"]["field_flippersound_sound-selector"][0].get<string>();
        json sound_json = json::parse(sound_string);
        string sound_name = sound_json["name"].get<string>();
        string sound_location = sound_json["location"].get<string>();*/

        string sound_name = json_object[name]["inputs"]["SOUND"][1];
        Block* sound = functionMap[json_object[sound_name]["opcode"]](json_object, sound_name).release();

        return make_unique<Play>(sound);
    };

    functionMap["flippersound_beepForTime"] = [&functionMap](const json& json_object, const string& name) {
        Block* time;
        if(json_object[name]["inputs"]["DURATION"][0] == 1){
            time = new BlankBlockDouble(stod(json_object[name]["inputs"]["DURATION"][1][1].get<string>()));
        } else {
            string time_name = json_object[name]["inputs"]["DURATION"][1];
            time = functionMap[json_object[time_name]["opcode"]](json_object, time_name).release();
        }
        string frequency_name = json_object[name]["inputs"]["NOTE"][1];
        Block* frequency = functionMap[json_object[frequency_name]["opcode"]](json_object, frequency_name).release();
        return make_unique<BeepForTime>(time, frequency);
    };

    functionMap["flippersound_beep"] = [&functionMap](const json& json_object, const string& name) {
        string frequency_name = json_object[name]["inputs"]["NOTE"][1];
        Block* frequency = functionMap[json_object[frequency_name]["opcode"]](json_object, frequency_name).release();
        return make_unique<Beep>(frequency);
    };

    functionMap["flippersound_stopSound"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<StopSound>();
    };

    functionMap["sound_changeeffectby"] = [&functionMap](const json& json_object, const string& name) {
        string effect = json_object[name]["fields"]["EFFECT"][0].get<string>();
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[value_name]["opcode"]](json_object, value_name).release();
        }
        return make_unique<ChangeEffectBy>(effect, value);
    };

    functionMap["sound_seteffectto"] = [&functionMap](const json& json_object, const string& name) {
        string effect = json_object[name]["fields"]["EFFECT"][0].get<string>();
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[value_name]["opcode"]](json_object, value_name).release();
        }
        return make_unique<SetEffectTo>(effect, value);
    };

    functionMap["sound_cleareffects"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<ClearEffects>();
    };

    functionMap["sound_changevolumeby"] = [&functionMap](const json& json_object, const string& name) {
        Block* volume;
        if(json_object[name]["inputs"]["VOLUME"][0] == 1){
            volume = new BlankBlockDouble(stod(json_object[name]["inputs"]["VOLUME"][1][1].get<string>()));
        } else {
            string volume_name = json_object[name]["inputs"]["VOLUME"][1];
            volume = functionMap[json_object[volume_name]["opcode"]](json_object, volume_name).release();
        }
        return make_unique<ChangeVolumeBy>(volume);
    };

    functionMap["sound_setvolumeto"] = [&functionMap](const json& json_object, const string& name) {
        Block* volume;
        if(json_object[name]["inputs"]["VOLUME"][0] == 1){
            volume = new BlankBlockDouble(stod(json_object[name]["inputs"]["VOLUME"][1][1].get<string>()));
        } else {
            string volume_name = json_object[name]["inputs"]["VOLUME"][1];
            volume = functionMap[json_object[volume_name]["opcode"]](json_object, volume_name).release();
        }
        return make_unique<SetVolumeTo>(volume);
    };

    functionMap ["sound_volume"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<Volume>();
    };
    //--------------------------------------------

    //Motor blocks
    functionMap["flippermotor_motorTurnForDirection"] = [&functionMap](const json& json_object, const string& name) {
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
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();
        
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        return make_unique<MotorTurnForDirection>(port, value, forward, unit);
    };

    functionMap["flippermotor_motorGoDirectionToPosition"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        string direction = json_object[name]["fields"]["DIRECTION"][0].get<string>();
        
        string position_name = json_object[name]["inputs"]["POSITION"][1];
        Block* position = functionMap[json_object[position_name]["opcode"]](json_object, position_name).release();
        return make_unique<MotorGoDirectionToPosition>(port, direction, position);
    };

    functionMap ["flippermotor_motorStartDirection"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

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

    functionMap ["flippermotor_motorStop"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        return make_unique<MotorStop>(port);
    };

    functionMap ["flippermotor_motorSetSpeed"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        Block* speed;
        if(json_object[name]["inputs"]["SPEED"][0] == 1){
            speed = new BlankBlockDouble(stod(json_object[name]["inputs"]["SPEED"][1][1].get<string>()));
        } else {
            string speed_name = json_object[name]["inputs"]["SPEED"][1];
            speed = functionMap[json_object[speed_name]["opcode"]](json_object, speed_name).release();
        }
        return make_unique<MotorSetSpeed>(port, speed);
    };

    functionMap ["flippermotor_absolutePosition"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        return make_unique<MotorPosition>(port);
    };

    functionMap ["flippermotor_speed"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        return make_unique<MotorSpeed>(port);
    };
    //---------------------------------------------

    // Control blocks
    functionMap["control_wait"] = [&functionMap](const json& json_object, const string& name) {
        Block* duration;
        if(json_object[name]["inputs"]["DURATION"][0] == 1){
            duration = new BlankBlockDouble(stod(json_object[name]["inputs"]["DURATION"][1][1].get<string>()));
        } else {
            string duration_name = json_object[name]["inputs"]["DURATION"][1];
            duration = functionMap[json_object[duration_name]["opcode"]](json_object, duration_name).release();
        }
        return make_unique<Wait>(duration);
    };

    functionMap["control_repeat"] = [&functionMap](const json& json_object, const string& name) {
        Block* times;
        if(json_object[name]["inputs"]["TIMES"][0] == 1){
            times = new BlankBlockInt(stoi(json_object[name]["inputs"]["TIMES"][1][1].get<string>()));
        } else {
            string times_name = json_object[name]["inputs"]["TIMES"][1];
            times = functionMap[json_object[times_name]["opcode"]](json_object, times_name).release();
        }

        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            string key = json_object[name]["inputs"]["SUBSTACK"][1];
            block_sequence = processBlock(json_object, key);
        }

        return make_unique<Repeat>(times, block_sequence);
    };

    functionMap["control_forever"] = [&functionMap](const json& json_object, const string& name) {
        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            string key = json_object[name]["inputs"]["SUBSTACK"][1];
            block_sequence = processBlock(json_object, key);
        }

        return make_unique<Forever>(block_sequence);
    };

    functionMap ["control_if"] = [&functionMap](const json& json_object, const string& name) {
        Block* condition = nullptr;
        if (json_object[name]["inputs"].contains("CONDITION")) {
            string condition_name = json_object[name]["inputs"]["CONDITION"][1];
            condition = functionMap[json_object[condition_name]["opcode"]](json_object, condition_name).release();
        }

        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            string key = json_object[name]["inputs"]["SUBSTACK"][1];
            block_sequence = processBlock(json_object, key);
        }

        return make_unique<If>(block_sequence, condition);
    };

    functionMap["control_if_else"] = [&functionMap](const json& json_object, const string& name) {
        Block* condition = nullptr;
        if (json_object[name]["inputs"].contains("CONDITION")) {
            string condition_name = json_object[name]["inputs"]["CONDITION"][1];
            condition = functionMap[json_object[condition_name]["opcode"]](json_object, condition_name).release();
        }

        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            string key = json_object[name]["inputs"]["SUBSTACK"][1];
            block_sequence = processBlock(json_object, key);
        }

        BlockSequence* else_block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK2")) {
            string key = json_object[name]["inputs"]["SUBSTACK2"][1];
            else_block_sequence = processBlock(json_object, key);
        }

        return make_unique<IfElse>(block_sequence, else_block_sequence, condition);
    };

    functionMap ["control_wait_until"] = [&functionMap](const json& json_object, const string& name) {
        Block* condition = nullptr;
        if (json_object[name]["inputs"].contains("CONDITION")) {
            string condition_name = json_object[name]["inputs"]["CONDITION"][1];
            condition = functionMap[json_object[condition_name]["opcode"]](json_object, condition_name).release();
        }

        return make_unique<WaitUntil>(condition);
    };

    functionMap ["control_repeat_until"] = [&functionMap](const json& json_object, const string& name) {
        Block* condition = nullptr;
        if (json_object[name]["inputs"].contains("CONDITION")) {
            string condition_name = json_object[name]["inputs"]["CONDITION"][1];
            condition = functionMap[json_object[condition_name]["opcode"]](json_object, condition_name).release();
        }

        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            string key = json_object[name]["inputs"]["SUBSTACK"][1];
            block_sequence = processBlock(json_object, key);
        }

        return make_unique<RepeatUntil>(condition, block_sequence);
    };

    functionMap ["flippercontrol_stopOtherStacks"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<StopOtherStacks>();
    };

    functionMap ["flippercontrol_stop"] = [&functionMap](const json& json_object, const string& name) {
        string option = json_object[name]["fields"]["STOP_OPTION"][0];
        return make_unique<ControlStop>(option);
    };
    //--------------------------------------------
    // Sensor blocks
    functionMap ["flippersensors_isColor"] = [&functionMap](const json& json_object, const string& name) {
        string color_name = json_object[name]["inputs"]["VALUE"][1];
        Block* color = functionMap[json_object[color_name]["opcode"]](json_object, color_name).release();
        
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        return make_unique<IsColor>(port, color);
    };

    functionMap ["flippersensors_color"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        return make_unique<SensorColor>(port);
    };

    functionMap ["flippersensors_isReflectivity"] = [&functionMap](const json& json_object, const string& name) {
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        string comparator = json_object[name]["fields"]["COMPARATOR"][0].get<string>();

        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        return make_unique<IsReflectivity>(port, comparator, value);
    };

    functionMap ["flippersensors_reflectivity"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        return make_unique<SensorReflectivity>(port);
    };

    functionMap ["flippersensors_isPressed"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string pressed = json_object[name]["fields"]["PRESSED"][0].get<string>();

        return make_unique<IsPressed>(port, pressed);
    }; 

    functionMap ["flippersensors_force"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();

        return make_unique<SensorForce>(port, unit);
    };

    functionMap ["flippersensors_isDistance"] = [&functionMap](const json& json_object, const string& name) {
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[value_name]["opcode"]](json_object, value_name).release();
        }

        string comparator = json_object[name]["fields"]["COMPARATOR"][0].get<string>();

        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();

        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        return make_unique<IsDistance>(port, comparator, value, unit);
    };

    functionMap ["flippersensors_distance"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();

        return make_unique<SensorDistance>(port, unit);
    };

    functionMap ["flippersensors_isTilted"] = [&functionMap](const json& json_object, const string& name) {
        string angle_name = json_object[name]["inputs"]["VALUE"][1];
        Block* angle = functionMap[json_object[angle_name]["opcode"]](json_object, angle_name).release();
        
        return make_unique<IsTilted>(angle);
    };

    functionMap ["flippersensors_isorientation"] = [&functionMap](const json& json_object, const string& name) {
        string orientation = json_object[name]["fields"]["ORIENTATION"][0].get<string>();
        return make_unique<IsOrientation>(orientation);
    };

    functionMap ["flippersensors_ismotion"] = [&functionMap](const json& json_object, const string& name) {
        string motion = json_object[name]["fields"]["MOTION"][0].get<string>();
        return make_unique<IsMotion>(motion);
    };

    functionMap ["flippersensors_orientationAxis"] = [&functionMap](const json& json_object, const string& name) {
        string axis = json_object[name]["fields"]["AXIS"][0].get<string>();
        return make_unique<OrientationAxis>(axis);
    };

    functionMap ["flippersensors_resetYaw"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<ResetYawAngle>();
    };

    functionMap ["flippersensors_buttonIsPressed"] = [&functionMap](const json& json_object, const string& name) {
        string button = json_object[name]["fields"]["BUTTON"][0].get<string>();
        string event = json_object[name]["fields"]["EVENT"][0].get<string>();
        return make_unique<IsButtonPressed>(button, event);
    };

    functionMap ["flippersensors_timer"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<Timer>();
    };

    functionMap ["flippersensors_resetTimer"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<ResetTimer>();
    };

    //---------------------------------------------
    // Miscelanious helper "blocks"
    functionMap["flippermove_movement-port-selector"] = [&functionMap](const json& json_object, const string& name) {
        string port = json_object[name]["fields"]["field_flippermove_multiple-port-selector"][0].get<string>();
        return make_unique<BlankBlockString>(port);
    };

    functionMap ["flippermotor_multiple-port-selector"] = [&functionMap](const json& json_object, const string& name) {
        string port = json_object[name]["fields"]["field_flippermotor_multiple-port-selector"][0].get<string>();
        return make_unique<BlankBlockString>(port);
    };

    functionMap["flippermotor_custom-angle"] = [&functionMap](const json& json_object, const string& name) {
        int angle = json_object[name]["fields"]["field_flippermotor_custom-angle"][0].get<int>();
        return make_unique<BlankBlockInt>(angle);
    };

    functionMap ["flipperevents_color-selector"] = [&functionMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flipperevents_color-selector"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(color));
    };

    functionMap ["flipperevents_color-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flipperevents_color-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(color);
    };

    functionMap ["flipperevents_force-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string force = json_object[name]["fields"]["field_flipperevents_force-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(force);
    };

    functionMap ["flipperevents_distance-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string distance = json_object[name]["fields"]["field_flipperevents_distance-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(distance);
    };

    functionMap ["flipperevents_custom-tilted"] = [&functionMap](const json& json_object, const string& name) {
        string tilt = json_object[name]["fields"]["field_flipperevents_custom-tilted"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(tilt));
    };

    functionMap ["flippersensors_color-selector"] = [&functionMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flippersensors_color-selector"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(color));
    };

    functionMap ["flippersensors_color-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flippersensors_color-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(color);
    };

    functionMap ["flippersensors_force-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string force = json_object[name]["fields"]["field_flippersensors_force-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(force);
    };

    functionMap ["flippersensors_distance-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string distance = json_object[name]["fields"]["field_flippersensors_distance-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(distance);
    };

    functionMap ["flippersensors_custom-tilted"] = [&functionMap](const json& json_object, const string& name) {
        string tilt = json_object[name]["fields"]["field_flippersensors_custom-tilted"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(tilt));
    };

    functionMap["flipperlight_matrix-5x5-brightness-image"] = [&functionMap](const json& json_object, const string& name) {
        string image = json_object[name]["fields"]["field_flipperlight_matrix-5x5-brightness-image"][0].get<string>();
        return make_unique<BlankBlockString>(image);
    };

    functionMap["flipperlight_menu_orientation"] = [&functionMap](const json& json_object, const string& name) {
        string orientation = json_object[name]["fields"]["orientation"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(orientation));
    };

    functionMap["flipperlight_color-selector-vertical"] = [&functionMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flipperlight_color-selector-vertical"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(color));
    };

    functionMap["flippersound_custom-piano"] = [&functionMap](const json& json_object, const string& name) {
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

    functionMap["flipperlight_matrix-pixel-index"] = [&functionMap](const json& json_object, const string& name) {
        string index = json_object[name]["fields"]["field_flipperlight_matrix-pixel-index"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(index));
    };

    functionMap["flipperlight_distance-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string distance = json_object[name]["fields"]["field_flipperlight_distance-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(distance);
    };

    functionMap["flipperlight_led-selector"] = [&functionMap](const json& json_object, const string& name) {
        string led = json_object[name]["fields"]["field_flipperlight_led-selector"][0].get<string>();
        return make_unique<BlankBlockString>(led);
    };

    functionMap["flippersound_sound-selector"] = [&functionMap](const json& json_object, const string& name) {
        string sound = json_object[name]["fields"]["field_flippersound_sound-selector"][0].get<string>();
        return make_unique<BlankBlockString>(sound);
    };


    return functionMap;
}

FunctionMap functionMap = createFunctionMap();


inline BlockSequence* processBlock(const json& blocks, string key) {
    auto curr_block = blocks[key];
    if (!curr_block.contains("opcode")) {
        cerr << "Error: Block is missing 'opcode'!" << endl;
        return nullptr;
    };
    auto curr_sequence_block = functionMap[curr_block["opcode"]](blocks, key).release();

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
        //cout << next_block["opcode"] << endl << curr_block["next"] << endl;
        auto next_sequence_block = functionMap[next_block["opcode"]](blocks, curr_block["next"]).release();
        curr_sequence_block->next = next_sequence_block;
        next_sequence_block->parent = curr_sequence_block;
        curr_sequence_block = next_sequence_block;
        curr_block = next_block;
    }

    return block_sequence;
}


#endif // BLOCKS_H