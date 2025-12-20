#ifndef VOLUME_HPP
#define VOLUME_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Volume : public Block {
public:
    Volume() : Block("Sound", "Volume") {}

    double execute(Robot& robot) override {
        return robot.volume;
    }
};

#endif