#ifndef DISPLAY_OFF_HPP
#define DISPLAY_OFF_HPP

#include "../common_block_includes.hpp"

using namespace std;

class DisplayOff : public Block {
public:
    DisplayOff() : Block("Display", "DisplayOff") {}

    double execute(Robot& robot) override {
        for(int i = 0; i < 5; ++i){
            for(int j = 0; j < 5; ++j){
                robot.pixel_display[i][j] = 0;
            }
        }
        robot.is_permanent_display = false;
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