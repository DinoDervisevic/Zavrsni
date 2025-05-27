#ifndef IS_TILTED_HPP
#define IS_TILTED_HPP

#include "../common_block_includes.hpp"

using namespace std;

class IsTilted : public Block {
    Block* angle;
public:
    IsTilted(Block* angle) : Block("Sensor", "IsTilted"), angle(angle) {}

    double execute(Robot& robot) override {
        if(is_number(angle->executeString(robot)) && angle->execute(robot) >= 0 && angle->execute(robot) <= 5){
            if (robot.tilt_angle == stoi(angle->executeString(robot))) {
                return 1;
            }
        }
        return 0;
    }

    string executeString(Robot& robot) override {
        if(is_number(angle->executeString(robot)) && angle->execute(robot) >= 0 && angle->execute(robot) <= 5){
            if (robot.tilt_angle == stoi(angle->executeString(robot))) {
                return "True";
            }
        }
        return "False";
    }
};

#endif