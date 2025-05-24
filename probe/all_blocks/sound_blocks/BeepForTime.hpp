#ifndef BEEP_FOR_TIME_HPP
#define BEEP_FOR_TIME_HPP

#include "../common_block_includes.hpp"

using namespace std;

class BeepForTime : public Block {
    Block* time;
    Block* frequency;
public:
    BeepForTime(Block* time, Block* frequency) : Block("Sound", "BeepForTime"), time(time), frequency(frequency) {}

    double execute(Robot& robot) override {
        if(!is_number(frequency->executeString(robot)) || frequency->execute(robot) < 48 || frequency->execute(robot) > 108){
            return 0;
        }
        if(!is_number(time->executeString(robot)) || time->execute(robot) <= 0){
            return 0;
        }
        robot.sound_playing = "beep " + to_string(static_cast<int>(frequency->execute(robot)));
        return time->execute(robot);
    }

    void finish(Robot& robot) override {
        robot.sound_playing = "";
        return;
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