#ifndef BROADCAST_HPP
#define BROADCAST_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Broadcast : public Block {
    Block* message;
public:
    Broadcast(Block* message) : Block("Event", "Broadcast"), message(message) {}

    double execute(Robot& robot) override {
        string str_message = message->executeString(robot);
        if (str_message.length() > 0) {
            robot.broadcasts.push_back(str_message);
            return 0;
        } else {
            return 0;
        }
    }
};


#endif