#ifndef WHEN_BUTTON_HPP
#define WHEN_BUTTON_HPP

#include "../common_block_includes.hpp"

using namespace std;

class WhenButton : public Block {
    string button;
    string event;
public:
    WhenButton(string button, string event) : Block("Event", "WhenButton"), button(button), event(event) {}

    double execute(Robot& robot) override {
        return 0;
    }

    bool done(Robot& robot) override {
        if (robot.buttons.find(button) != robot.buttons.end() && robot.buttons[button] == event) {
            return true;
        }
        return false;
    }
};

#endif