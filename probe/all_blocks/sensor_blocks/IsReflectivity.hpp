#ifndef IS_REFLECTIVITY_HPP
#define IS_REFLECTIVITY_HPP

#include "../common_block_includes.hpp"

using namespace std;

class IsReflectivity : public Block {
    Block* port;
    string comparator;
    Block* value;
public:
    IsReflectivity(Block* port, string comparator, Block* value) : Block("Sensor", "IsReflectivity"), port(port), comparator(comparator), value(value) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement reflectivity detection
    }
};

#endif