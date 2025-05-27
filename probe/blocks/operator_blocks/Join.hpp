#ifndef JOIN_HPP
#define JOIN_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Join : public Block {
    Block* value1;
    Block* value2;
public:
    Join(Block* value1, Block* value2) : Block("Operator", "Join"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return 0;
    }

    string executeString(Robot& robot) override {
        return value1->executeString(robot) + value2->executeString(robot);
    }
};

#endif