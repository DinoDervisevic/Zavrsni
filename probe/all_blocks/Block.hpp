#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <string>
#include <iostream>

using namespace std;

#include "../robot.hpp"

class Block {
    public:
        Block* next = nullptr;
        Block* parent = nullptr;
        string type;
        string name;
        
        Block(const string& type, string name) : type(type), name(name) {}
    
        // changes the state of the robot and returns the number of seconds it took to execute the block (0 in case on instantaneus blocks such as speed change)
        virtual double execute(Robot& robot) = 0; 
        virtual void finish(Robot& robot){
            return;
        }
        virtual string executeString(Robot& robot) {
            return to_string(execute(robot));
        }
        virtual ~Block() = default;
        virtual bool done(Robot& robot) {
            return true;
        }

        virtual void deal_with_interference(Robot& robot, BlockSequence* sequence) {
            return;
        }

        //Only for teh first two motor blocks
        virtual void stop_motors(Robot& robot, string port) {
            return;
        }
};

#endif