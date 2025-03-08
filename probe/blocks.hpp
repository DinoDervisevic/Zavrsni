#ifndef BLOCKS_H
#define BLOCKS_H

#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include "json.hpp"
#include "robot.hpp"
#include "functions.hpp"

using namespace std;

using json = nlohmann::json;

class Block {
public:
    Block* next = nullptr;
    Block* parent = nullptr;
    string type;
    string name;
    
    Block(const string& type, string name) : type(type), name(name) {}

    // changes the state of the robot and returns the number of seconds it took to execute the block (0 in case on instantaneus blocks such as speed change)
    virtual int execute(Robot& robot) = 0; 
    virtual ~Block() = default;
};

class Move : public Block {
    bool forward;
    double value;
    string unit;
public:
    Move(bool forward, double value, string unit) : Block("Move", "Move"), forward(forward), value(value), unit(unit) {}

    int execute(Robot& robot) override {
        double time = convert_to_seconds(robot, unit, value);
        if(forward){
            robot.v1 = robot.movement_speed;
            robot.v2 = robot.movement_speed;
        } else {
            robot.v1 = -robot.movement_speed;
            robot.v2 = -robot.movement_speed;
        }
        return time;
    }
    
};

class MotorTurnForDirection : public Block {
public:
    double speed;
    MotorTurnForDirection(double speed) : Block("Motor", "MotorTurnForDirection"), speed(speed) {}

    int execute(Robot& robot) override {
        robot.v1 = speed;
        robot.v2 = speed;
        std::cout << "Robot se krece naprijed brzinom: " << speed << std::endl;
    }
};

class MotorStop : public Block {
public:
int execute(Robot& robot) override {
        robot.v1 = 0.0;
        robot.v2 = 0.0;
        return 0;
    }
};

class LightDisplayText : public Block {
public:
    std::string text;
    LightDisplayText(const std::string& text) : Block("Light", "LightDisplayText"), text(text) {}

    int execute(Robot& robot) override {
        robot.display_state = text;
        std::cout << "Prikazujem tekst: " << robot.display_state << std::endl;
    }
};

class LightDisplayOff : public Block {
public:
    int execute(Robot& robot) override {
        robot.display_state = "";
        std::cout << "Prikaz je iskljucen." << std::endl;
    }
};

using FunctionMap = std::map<std::string, std::function<std::unique_ptr<Block>(json)>>;

FunctionMap createFunctionMap() {
    FunctionMap functionMap;
    functionMap["flippermotor_motorTurnForDirection"] = [](json inputs) {
        double speed = inputs["VALUE"][1][1].get<double>();
        return std::make_unique<MotorTurnForDirection>(speed);
    };
    functionMap["flippermotor_motorStop"] = [](json inputs) {
        return std::make_unique<MotorStop>();
    };
    functionMap["flipperlight_lightDisplayText"] = [](json inputs) {
        std::string text = inputs["TEXT"][1][1].get<std::string>();
        return std::make_unique<LightDisplayText>(text);
    };
    functionMap["flipperlight_lightDisplayOff"] = [](json inputs) {
        return std::make_unique<LightDisplayOff>();
    };
    // Dodajte ostale funkcije prema potrebi
    return functionMap;
}

#endif // BLOCKS_H