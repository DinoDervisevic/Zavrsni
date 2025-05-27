#ifndef IS_MOTION_HPP
#define IS_MOTION_HPP

#include "../common_block_includes.hpp"

using namespace std;

class IsMotion : public Block {
    string motion;
public: 
    IsMotion(string motion) : Block("Sensor", "IsMotion"), motion(motion) {}

    double execute(Robot& robot) override {
        return 0; // TODO: i don't think i have basically any way of implementing this
    }
};

#endif