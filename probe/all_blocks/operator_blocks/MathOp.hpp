#ifndef MATH_OP_HPP
#define MATH_OP_HPP

#include "../common_block_includes.hpp"

using namespace std;

class MathOp : public Block {
    Block* value;
    string function_name;
public:
    MathOp(Block* value, string function_name) : Block("Operator", "MathOp"), value(value), function_name(function_name) {}

    double execute(Robot& robot) override {
        if(function_name == "abs"){
            return abs(value->execute(robot));
        } else if(function_name == "floor"){
            return floor(value->execute(robot));
        } else if (function_name == "ceiling"){
            return ceil(value->execute(robot));
        } else if (function_name == "sqrt"){
            return sqrt(value->execute(robot));
        } else if(function_name == "sin"){
            return sin(value->execute(robot));
        } else if(function_name == "cos"){
            return cos(value->execute(robot));
        } else if(function_name == "tan"){
            return tan(value->execute(robot));
        } else if(function_name == "asin"){
            return asin(value->execute(robot));
        } else if(function_name == "acos"){
            return acos(value->execute(robot));
        } else if(function_name == "atan"){
            return atan(value->execute(robot));
        } else if(function_name == "ln"){
            return log(value->execute(robot));
        } else if(function_name == "log"){
            return log10(value->execute(robot));
        } else if(function_name == "e ^"){
            return exp(value->execute(robot));
        } else if(function_name == "10 ^"){
            return pow(10, value->execute(robot));
        }
        return 0;
    }
};

#endif