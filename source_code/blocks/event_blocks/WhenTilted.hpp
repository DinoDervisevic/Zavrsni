#ifndef WHEN_TILTED_HPP
#define WHEN_TILTED_HPP

#include "../common_block_includes.hpp"

using namespace std;

class WhenTilted : public Block {
    Block* angle;
public:
    WhenTilted(Block* angle) : Block("Event", "WhenTilted"), angle(angle) {}

    double execute(Robot& robot) override {
        return 0;
    }

    bool done(Robot& robot) override {
        if(is_number(angle->executeString(robot)) && angle->execute(robot) >= 0 && angle->execute(robot) <= 5){
            if (robot.tilt_angle == stod(angle->executeString(robot))) {
                return true;
            }
        }
        return false;
    }
};

#endif