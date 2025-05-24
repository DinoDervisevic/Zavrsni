#ifndef SET_MOTOR_ROTATION_HPP
#define SET_MOTOR_ROTATION_HPP

#include "../common_block_includes.hpp"

using namespace std;

class SetMotorRotation : public Block { // TODO : ja sam nekih 90% siguran da ovo radi doslovno nista
    string unit;
    Block* value;
public:
    SetMotorRotation(string unit, Block* value) : Block("Move", "SetMotorRotation"), unit(unit), value(value) {}

    double execute(Robot& robot) override {
        return 0;
    }
};

#endif