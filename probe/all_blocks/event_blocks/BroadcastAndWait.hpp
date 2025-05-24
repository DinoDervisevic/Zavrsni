#ifndef BROADCAST_AND_WAIT_HPP
#define BROADCAST_AND_WAIT_HPP

#include "../common_block_includes.hpp"

using namespace std;

class BroadcastAndWait : public Block {
    Block* message;
public:
    BroadcastAndWait(Block* message) : Block("Event", "BroadcastAndWait"), message(message) {}

    double execute(Robot& robot) override {
        string str_message = message->executeString(robot);
        if (str_message.length() > 0) robot.broadcasts.push_back(str_message);
        return 0;
    }

    bool done(Robot& robot) override {
        if (find(robot.finished_broadcasts.begin(), robot.finished_broadcasts.end(), message->executeString(robot)) != robot.finished_broadcasts.end()) {
            return true;
        } else {
            return false;
        }
    }
};

#endif