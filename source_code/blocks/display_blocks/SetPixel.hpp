#ifndef SET_PIXEL_HPP
#define SET_PIXEL_HPP

#include "../common_block_includes.hpp"

using namespace std;

class SetPixel : public Block {
    Block* x;
    Block* y;
    Block* brightness;
public:
    SetPixel(Block* x, Block* y, Block* brightness) : Block("Display", "SetPixel"), x(x), y(y), brightness(brightness) {}

    double execute(Robot& robot) override {
        if(!is_number(x->executeString(robot)) || !is_number(y->executeString(robot))
        || x->execute(robot) < 1 || x->execute(robot) > 5
        || y->execute(robot) < 1 || y->execute(robot) > 5
        || !is_number(brightness->executeString(robot))
        || brightness->execute(robot) < 0 || brightness->execute(robot) > 100){
            return 0;
        }
        robot.pixel_display[static_cast<int>(y->execute(robot)-1)][static_cast<int>(x->execute(robot)-1)] = min(brightness->execute(robot), 100.0);
        robot.permanent_pixel_display[static_cast<int>(y->execute(robot)-1)][static_cast<int>(x->execute(robot)-1)] = min(brightness->execute(robot), 100.0);
        
        return 0;
    }
};

#endif