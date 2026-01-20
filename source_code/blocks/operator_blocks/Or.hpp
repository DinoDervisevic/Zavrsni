#ifndef OR_HPP
#define OR_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Or : public Block {
    Block* value1;
    Block* value2;
public:
    Or(Block* value1, Block* value2) : Block("Operator", "Or"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        if (value1 == nullptr || value2 == nullptr) {
            return 0;
        }
        return value1->execute(robot) || value2->execute(robot);
    }

    string executeString(Robot& robot) override {
        if (value1 == nullptr || value2 == nullptr) {
            return "False";
        }
        if(value1->execute(robot) || value2->execute(robot)){
            return "True";
        } else {
            return "False";
        }
    }
};

#endif