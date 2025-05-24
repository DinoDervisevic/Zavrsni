#ifndef CLEAR_EFFECTS_HPP
#define CLEAR_EFFECTS_HPP

#include "../common_block_includes.hpp"

using namespace std;

class ClearEffects : public Block { // TODO : nemam pojma sto ovo tocno radi
public:
    ClearEffects() : Block("Sound", "ClearEffects") {}

    double execute(Robot& robot) override {
        return 0;
    }
};

#endif