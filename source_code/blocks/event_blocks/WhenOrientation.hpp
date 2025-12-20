#ifndef WHEN_ORIENTATION_HPP
#define WHEN_ORIENTATION_HPP

#include "../common_block_includes.hpp"

using namespace std;

class WhenOrientation : public Block {
    string orientation;
public:
    WhenOrientation(string orientation) : Block("Event", "WhenOrientation"), orientation(orientation) {}

    double execute(Robot& robot) override {
        return 0;
    }

    bool done(Robot& robot) override {
        if (robot.orientation == orientation) {
            return true;
        }
        return false;
    }
};

#endif