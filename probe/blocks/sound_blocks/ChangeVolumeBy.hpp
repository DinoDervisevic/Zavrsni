#ifndef CHANGE_VOLUME_BY_HPP
#define CHANGE_VOLUME_BY_HPP

#include "../common_block_includes.hpp"

using namespace std;

class ChangeVolumeBy : public Block { // TODO : provjeri
    Block* value;
public:
    ChangeVolumeBy(Block* value) : Block("Sound", "ChangeVolumeBy"), value(value) {}

    double execute(Robot& robot) override {
        if(!is_number(value->executeString(robot))){
            return 0;
        }
        robot.volume += value->execute(robot);
        return 0;
    }
};

#endif