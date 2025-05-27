#ifndef SET_EFFECT_TO_HPP
#define SET_EFFECT_TO_HPP

#include "../common_block_includes.hpp"

using namespace std;

class SetEffectTo : public Block { // TODO : nemam pojma sto ovo tocno radi
    string effect;
    Block* value;
public:
    SetEffectTo(string effect, Block* value) : Block("Sound", "SetEffectTo"), effect(effect), value(value) {}

    double execute(Robot& robot) override {
        return 0;
    }
};

#endif