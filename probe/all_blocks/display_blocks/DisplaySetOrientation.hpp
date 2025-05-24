#ifndef DISPLAY_SET_ORIENTATION_HPP
#define DISPLAY_SET_ORIENTATION_HPP

#include "../common_block_includes.hpp"

using namespace std;

class DisplaySetorientation : public Block { // TODO : provjeri jer nisam 100% convinced
    Block* orientation;
public:
    DisplaySetorientation(Block* orientation) : Block("Display", "DisplaySetorientation"), orientation(orientation) {}
    double execute(Robot& robot) override {
        if(!is_number(orientation->executeString(robot)) || orientation->execute(robot) < 1 || orientation->execute(robot) > 4){
            return 0;
        }
        while(robot.absolute_image_position != static_cast<int>(orientation->execute(robot)-1)){
            rotate_matrix_right(robot);
        }
        return 0;
    }
};

#endif