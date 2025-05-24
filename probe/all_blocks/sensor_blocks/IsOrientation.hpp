#ifndef IS_ORIENTATION_HPP
#define IS_ORIENTATION_HPP

#include "../common_block_includes.hpp"

using namespace std;

class IsOrientation : public Block {
    string orientation;
public:
    IsOrientation(string orientation) : Block("Sensor", "IsOrientation"), orientation(orientation) {}

    double execute(Robot& robot) override {
        if (robot.orientation == orientation) {
            return 1;
        }
        return 0;
    }

    string executeString(Robot& robot) override {
        if (robot.orientation == orientation) {
            return "True";
        }
        return "False";
    }
};

#endif