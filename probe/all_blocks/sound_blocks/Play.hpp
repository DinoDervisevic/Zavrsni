#ifndef PLAY_HPP
#define PLAY_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Play : public Block {
    Block* sound_name;
public:
    Play(Block* sound_name) : Block("Sound", "Play"), sound_name(sound_name){};
    double execute(Robot& robot) override {
        robot.sound_playing = sound_name->executeString(robot);
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