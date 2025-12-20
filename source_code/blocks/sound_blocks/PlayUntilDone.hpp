#ifndef PLAY_UNTIL_DONE_HPP
#define PLAY_UNTIL_DONE_HPP

#include "../common_block_includes.hpp"

using namespace std;

//TODO : playing sounds ne znam kako napraviti jos
class PlayUntilDone : public Block {
    Block* sound_name;
public:
    PlayUntilDone(Block* sound_name) : Block("Sound", "PlayUntilDone"), sound_name(sound_name){};
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