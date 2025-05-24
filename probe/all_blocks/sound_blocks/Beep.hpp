#ifndef BEEP_HPP
#define BEEP_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Beep : public Block {
    Block* frequency;
public:
    Beep(Block* frequency) : Block("Sound", "Beep"), frequency(frequency) {}

    double execute(Robot& robot) override {
        if(!is_number(frequency->executeString(robot)) || frequency->execute(robot) < 48 || frequency->execute(robot) > 108){
            return 0;
        }
        robot.sound_playing = "beep " + to_string(static_cast<int>(frequency->execute(robot)));
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