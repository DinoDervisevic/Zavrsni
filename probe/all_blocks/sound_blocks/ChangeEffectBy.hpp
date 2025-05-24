#ifndef CHANGE_EFFECT_BY_HPP
#define CHANGE_EFFECT_BY_HPP

#include "../common_block_includes.hpp"

using namespace std;

class ChangeEffectBy : public Block { // TODO : nemam pojma sto ovo tocno radi
    string effect;
    Block* value;
public:
    ChangeEffectBy(string effect, Block* value) : Block("Sound", "ChangeEffectBy"), effect(effect), value(value) {}

    double execute(Robot& robot) override {
        return 0;
    }
};

#endif