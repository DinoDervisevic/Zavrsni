#ifndef BLOCKS_H
#define BLOCKS_H

#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include "json.hpp"
#include "robot.h"

using json = nlohmann::json;

class Block {
public:
    Block* next = nullptr;
    Block* parent = nullptr;

    virtual void execute(Robot& robot) = 0;
    virtual ~Block() = default;
};

class MotorTurnForDirection : public Block {
public:
    double speed;
    MotorTurnForDirection(double speed) : speed(speed) {}

    void execute(Robot& robot) override {
        robot.v1 = speed;
        robot.v2 = speed;
        std::cout << "Robot se krece naprijed brzinom: " << speed << std::endl;
    }
};

class MotorStop : public Block {
public:
    void execute(Robot& robot) override {
        robot.v1 = 0.0;
        robot.v2 = 0.0;
        std::cout << "Robot je stao." << std::endl;
    }
};

class LightDisplayText : public Block {
public:
    std::string text;
    LightDisplayText(const std::string& text) : text(text) {}

    void execute(Robot& robot) override {
        robot.display_state = text;
        std::cout << "Prikazujem tekst: " << robot.display_state << std::endl;
    }
};

class LightDisplayOff : public Block {
public:
    void execute(Robot& robot) override {
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