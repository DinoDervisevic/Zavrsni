#ifndef DISPLAY_ROTATE_HPP
#define DISPLAY_ROTATE_HPP

#include "../common_block_includes.hpp"

using namespace std;

class DisplayRotate : public Block {
    bool forward;
public:
    DisplayRotate(bool forward) : Block("Display", "DisplayRotate"), forward(forward) {}
    double execute(Robot& robot) override { // TODO : provjeri
        if(forward) rotate_matrix_right(robot);
        else rotate_matrix_left(robot);
        return 0;
    }
};

#endif