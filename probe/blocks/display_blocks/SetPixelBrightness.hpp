#ifndef SET_PIXEL_BRIGHTNESS_HPP
#define SET_PIXEL_BRIGHTNESS_HPP

#include "../common_block_includes.hpp"

using namespace std;

class SetPixelbrightness : public Block {
    Block* brightness;
public:
    SetPixelbrightness(Block* brightness) : Block("Display", "SetPixelbrightness"), brightness(brightness) {}

    double execute(Robot& robot) override {
        if(!is_number(brightness->executeString(robot))){
            return 0;
        }
        double b = brightness->execute(robot);
        b = min(b, 100.0);
        b = max(b, 0.0);
        robot.pixel_display_brightness = b;

        return 0;
    }
};

#endif