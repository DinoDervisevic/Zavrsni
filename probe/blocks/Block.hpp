#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <string>
#include <iostream>

using namespace std;

#include "../robot/Robot.hpp"

class Block {
    public:
        Block* next = nullptr;
        Block* parent = nullptr;
        string type;
        string name;
        
        Block(const string& type, string name) : type(type), name(name) {}
    
        // return the time in seconds that the block takes to execute
        virtual double execute(Robot& robot) = 0; 

        virtual void finish(Robot& robot){
            return;
        }
        
        virtual string executeString(Robot& robot) {
            return to_string(execute(robot));
        }
        
        virtual bool done(Robot& robot) {
            return true;
        }

        virtual void deal_with_interference(Robot& robot, BlockSequence* sequence) {
            return;
        }

        virtual void stop_motors(Robot& robot, string port) {
            return;
        }

        virtual ~Block() = default;
};

#endif