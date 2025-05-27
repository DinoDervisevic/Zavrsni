#ifndef WHEN_GESTURE_HPP
#define WHEN_GESTURE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class WhenGesture : public Block {
    string gesture;
public:
    WhenGesture(string gesture) : Block("Event", "WhenGesture"), gesture(gesture) {}

    double execute(Robot& robot) override {
        return 0;
    }

    bool done(Robot& robot) override {
        if (robot.gesture == gesture) {
            return true;
        }
        return false;
    }
};

#endif