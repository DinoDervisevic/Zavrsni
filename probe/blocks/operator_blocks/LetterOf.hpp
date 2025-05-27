#ifndef LETTER_OF_HPP
#define LETTER_OF_HPP

#include "../common_block_includes.hpp"

using namespace std;

class LetterOf : public Block {
    Block* value1;
    Block* value2;
public:
    LetterOf(Block* value1, Block* value2) : Block("Operator", "LetterOf"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return 0;
    }

    string executeString(Robot& robot) override {
        return value1->executeString(robot).substr(value2->execute(robot), 1);
    }
};

#endif