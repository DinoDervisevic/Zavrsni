#ifndef VALUESTORER_HPP
#define VALUESTORER_HPP

#include <vector>
#include <memory>
#include "blocks.hpp"
#include "robot.hpp"
#include "json.hpp"
#include "functions.hpp"

using namespace std;

using json = nlohmann::json;

FunctionMap functionMap = createFunctionMap();

class ValueStorer {
public:
    virtual double get_value(Robot& robot) = 0;
    virtual ~ValueStorer() = default;
};

class IntValueStorer : public ValueStorer {
    int value;
    Block* block;
public:
    IntValueStorer(const json& json_object, const string& name){
        if(is_integer(name)){
            value = stoi(name);
        } else {
            block = functionMap[json_object[name]["opcode"]](json_object, name).release();;
        }
    }

    double get_value(Robot& robot) override {
        if (block) {
            double result = block->execute(robot);
            if (result != static_cast<int>(result)) {
                throw runtime_error("Expected integer value, but got non-integer value.");
            }
            return static_cast<int>(result);
        }
        return value;
    }
};

class DoubleValueStorer : public ValueStorer {
    double value;
    Block* block;
public:
    DoubleValueStorer(const json& json_object, const string& name) {
        if(is_number(name)){
            value = stod(name);
        } else {
            block = functionMap[json_object[name]["opcode"]](json_object, name).release();
        }
    }

    double get_value(Robot& robot) override {
        if (block) {
            return block->execute(robot);
        }
        return value;
    }
};

class ConditionalValueStorer : public ValueStorer {
    bool condition;
    Block* block;
public:
    ConditionalValueStorer(bool condition, Block* block) : condition(condition), block(block) {}

    double get_value(Robot& robot) override {
        if (block) {
            return block->execute(robot);
        }
        return condition;
    }
};

#endif // VALUESTORER_HPP