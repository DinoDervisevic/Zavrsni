#ifndef STOP_SOUND_HPP
#define STOP_SOUND_HPP

#include "../common_block_includes.hpp"

using namespace std;

class StopSound : public Block {
public:
    StopSound() : Block("Sound", "StopSound") {}

    double execute(Robot& robot) override {
        robot.sound_playing = "";
        return 0;
    }

    void deal_with_interference(Robot& robot, BlockSequence* sequence) override {
        Block* block = sequence->get_current_block();
        if (block->name == "BeepForTime" || block->name == "PlayUntilDone") {
            block->finish(robot);
            sequence->set_time_left(0);
        }
    }
};

#endif