#ifndef CENTER_BUTTON_LIGHT_HPP
#define CENTER_BUTTON_LIGHT_HPP

#include "../common_block_includes.hpp"

using namespace std;

class CenterButtonLight : public Block {
    Block* color;

public:
    CenterButtonLight(Block* color) : Block("Display", "CenterButtonLight"), color(color) {}

    double execute(Robot& robot) override {
        if(!is_number(color->executeString(robot))) robot.button_color = 0;
        else robot.button_color = color->execute(robot);
        return 0;
    }
};

#endif