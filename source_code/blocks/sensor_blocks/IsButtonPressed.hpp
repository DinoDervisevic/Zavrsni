#ifndef IS_BUTTON_PRESSED_HPP
#define IS_BUTTON_PRESSED_HPP

#include "../common_block_includes.hpp"

using namespace std;

class IsButtonPressed : public Block {
    string button;
    string event;
public:
    IsButtonPressed(string button, string event) : Block("Sensor", "IsButtonPressed"), button(button), event(event) {}

    double execute(Robot& robot) override {
        if (robot.buttons.find(button) != robot.buttons.end() && robot.buttons[button] == event) {
            return 1;
        }
        return 0;
    }

    string executeString(Robot& robot) override {
        if (robot.buttons.find(button) != robot.buttons.end() && robot.buttons[button] == event) {
            return "True";
        }
        return "False";
    }
};

#endif