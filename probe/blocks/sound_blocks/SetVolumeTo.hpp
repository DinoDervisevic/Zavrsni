#ifndef SET_VOLUME_TO_HPP
#define SET_VOLUME_TO_HPP

#include "../common_block_includes.hpp"

using namespace std;

class SetVolumeTo : public Block {// TODO : provjeri
    Block* value;
public:
    SetVolumeTo(Block* value) : Block("Sound", "SetVolumeTo"), value(value) {}

    double execute(Robot& robot) override {
        if(!is_number(value->executeString(robot))){
            return 0;
        }
        robot.volume = value->execute(robot);
        return 0;
    }
};

#endif