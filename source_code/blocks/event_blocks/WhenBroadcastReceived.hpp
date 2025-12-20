#ifndef WHEN_BROADCAST_RECEIVED_HPP
#define WHEN_BROADCAST_RECEIVED_HPP

#include "../common_block_includes.hpp"

using namespace std;

class WhenBroadcastReceived : public Block { // TODO: napravi broadcast do kraja sto se tice paralelizma
    string message;
public:
    WhenBroadcastReceived(string message) : Block("Event", "WhenBroadcastReceived"), message(message) {}

    double execute(Robot& robot) override {
        return 0;
    }

    bool done(Robot& robot) override {
        if (find(robot.broadcasts.begin(), robot.broadcasts.end(), message) != robot.broadcasts.end()) {
            return true;
        }
        return false;
    }
};

#endif