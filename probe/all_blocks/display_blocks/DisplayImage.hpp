#ifndef DISPLAY_IMAGE_HPP
#define DISPLAY_IMAGE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class DisplayImage : public Block {
    Block* image;
public:
    DisplayImage(Block* image) : Block("Display", "DisplayImage"), image(image) {}

    double execute(Robot& robot) override {
        string str_image = image->executeString(robot);
        for(int i = 0; i < 25; ++i){
            if(i >= str_image.length()){
                str_image += "0";
            }
            if(!isdigit(str_image[i])) str_image[i] = '0';
        }
        for(int i = 0; i < 5; ++i){
            for(int j = 0; j < 5; ++j){
                robot.permanent_pixel_display[i][j] = stoi(string(1, str_image[i*5 + j]))* robot.pixel_display_brightness / 9;
            }
        }
        robot.is_permanent_display = true;
        return 0;
    }

    void deal_with_interference(Robot& robot, BlockSequence* sequence) override {
        Block* block = sequence->get_current_block();
        if (block->name == "DisplayImageForTime" || block->name == "DisplayText") {
            block->finish(robot);
            sequence->set_time_left(0);
        }
    }
};

#endif