#ifndef OPERATOR_BLOCKS_PARSING_HPP
#define OPERATOR_BLOCKS_PARSING_HPP

#include <map>
#include <functional>
#include <memory>
#include <string>

#include "../../blocks/Block.hpp"
#include "../../blocks/BlockSequence.hpp"
#include "../../external/json.hpp"
#include "../../blocks/operator_blocks/operator_blocks_include.hpp"
#include "../../blocks/blank_blocks/blank_blocks_include.hpp"
#include "../string_conversion.hpp"

using namespace std;
using json = nlohmann::json;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;

FunctionMap createOperatorFunctionMap(FunctionMap& globalMap) {
    FunctionMap functionMap;

     // Operator blocks
    functionMap["operator_random"] = [&globalMap](const json& json_object, const string& name) {
        Block* from;
        Block* to;
        if(json_object[name]["inputs"]["FROM"][0] == 1){
            from = new BlankBlockInt(parseInt(json_object[name]["inputs"]["FROM"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["FROM"][1];
            from = globalMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }

        if(json_object[name]["inputs"]["TO"][0] == 1){
            
            to = new BlankBlockInt(parseInt(json_object[name]["inputs"]["TO"][1][1].get<string>()));
        } else {
            string to_name = json_object[name]["inputs"]["TO"][1];
            to = globalMap[json_object[to_name]["opcode"]](json_object, to_name).release();
        }
        return make_unique<PickRandom>(from, to);
    };

    functionMap["operator_add"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            
            value1 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            
            value2 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Add>(value1, value2);
    };

    functionMap["operator_subtract"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            
            value1 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            
            value2 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Subtract>(value1, value2);
    };

    functionMap["operator_multiply"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            
            value1 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
           
            value2 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Multiply>(value1, value2);
    };

    functionMap["operator_divide"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            
            value1 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            
            value2 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Divide>(value1, value2);
    };

    functionMap["operator_lt"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["OPERAND1"][0] == 1){
            
            value1 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["OPERAND1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["OPERAND2"][0] == 1){
            
            value2 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["OPERAND2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<LessThan>(value1, value2);
    };

    functionMap["operator_equals"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["OPERAND1"][0] == 1){
            
            value1 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["OPERAND1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["OPERAND2"][0] == 1){
            
            value2 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["OPERAND2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Equals>(value1, value2);
    };

    functionMap["operator_gt"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["OPERAND1"][0] == 1){
            value1 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["OPERAND1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["OPERAND2"][0] == 1){
            value2 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["OPERAND2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<GreaterThan>(value1, value2);
    };

    functionMap["operator_and"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1 = nullptr;
        Block* value2 = nullptr;
        if (json_object[name]["inputs"].contains("OPERAND1")) {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        } 

        if (json_object[name]["inputs"].contains("OPERAND2")) {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        } 
        return make_unique<And>(value1, value2);
    };

    functionMap["operator_or"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1 = nullptr;
        Block* value2 = nullptr;
        if (json_object[name]["inputs"].contains("OPERAND1")) {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        } 

        if (json_object[name]["inputs"].contains("OPERAND2")) {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        } 
        return make_unique<Or>(value1, value2);
    };

    functionMap["operator_not"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1 = nullptr;
        if (json_object[name]["inputs"].contains("OPERAND")) {
            string value1_name = json_object[name]["inputs"]["OPERAND"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }
        return make_unique<Not>(value1);
    };

    functionMap["flipperoperator_isInBetween"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        Block* value3;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value1 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["VALUE"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["LOW"][0] == 1){
            value2 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["LOW"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["LOW"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }

        if(json_object[name]["inputs"]["HIGH"][0] == 1){
            value3 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["HIGH"][1][1].get<string>()));
        } else {
            string value3_name = json_object[name]["inputs"]["HIGH"][1];
            value3 = globalMap[json_object[value3_name]["opcode"]](json_object, value3_name).release();
        }
        return make_unique<IsInBetween>(value1, value2, value3);
    };

    functionMap["operator_join"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["STRING1"][0] == 1){
            value1 = new BlankBlockString(json_object[name]["inputs"]["STRING1"][1][1].get<string>());
        } else {
            string value1_name = json_object[name]["inputs"]["STRING1"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["STRING2"][0] == 1){
            value2 = new BlankBlockString(json_object[name]["inputs"]["STRING2"][1][1].get<string>());
        } else {
            string value2_name = json_object[name]["inputs"]["STRING2"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Join>(value1, value2);
    };

    functionMap["operator_letterof"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["STRING"][0] == 1){
            value2 = new BlankBlockString(json_object[name]["inputs"]["STRING"][1][1].get<string>());
        } else {
            string value1_name = json_object[name]["inputs"]["STRING"][1];
            value2 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["LETTER"][0] == 1){
            value1 = new BlankBlockInt(parseInt(json_object[name]["inputs"]["LETTER"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["LETTER"][1];
            value1 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<LetterOf>(value1, value2);
    };

    functionMap["operator_length"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        if(json_object[name]["inputs"]["STRING"][0] == 1){
            value1 = new BlankBlockString(json_object[name]["inputs"]["STRING"][1][1].get<string>());
        } else {
            string value1_name = json_object[name]["inputs"]["STRING"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }
        return make_unique<LengthOf>(value1);
    };

    functionMap["operator_contains"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["STRING1"][0] == 1){
            value1 = new BlankBlockString(json_object[name]["inputs"]["STRING1"][1][1].get<string>());
        } else {
            string value1_name = json_object[name]["inputs"]["STRING1"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["STRING2"][0] == 1){
            value2 = new BlankBlockString(json_object[name]["inputs"]["STRING2"][1][1].get<string>());
        } else {
            string value2_name = json_object[name]["inputs"]["STRING2"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Contains>(value1, value2);
    };

    functionMap["operator_mod"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            value1 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            value2 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = globalMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Modulus>(value1, value2);
    };

    functionMap["operator_round"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        if(json_object[name]["inputs"]["NUM"][0] == 1){
            value1 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["NUM"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }
        return make_unique<Round>(value1);
    };

    functionMap["operator_mathop"] = [&globalMap](const json& json_object, const string& name) {
        Block* value1;
        if(json_object[name]["inputs"]["NUM"][0] == 1){
            value1 = new BlankBlockDouble(parseDouble(json_object[name]["inputs"]["NUM"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM"][1];
            value1 = globalMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }
        return make_unique<MathOp>(value1, json_object[name]["fields"]["OPERATOR"][0]);
    };
    //--------------------------------------------

    return functionMap;

}

#endif