#ifndef REPEAT_HPP
#define REPEAT_HPP

#include "../common_block_includes.hpp"

using namespace std;

class Repeat : public Block {
    Block* times;
    int claculated_times;
    bool first_time = true;
    BlockSequence* block_sequence;
    int counter = 0;
    bool when_done = false;
public:
    Repeat(Block* times, BlockSequence* block_sequence) : Block("Control", "Repeat"), times(times), block_sequence(block_sequence), counter(0) {}

    double execute(Robot& robot) override {
        if (block_sequence == nullptr) {
            return 0;
        }
        if (first_time) {
            block_sequence->reset(robot);
            claculated_times = std::ceil(times->execute(robot));
            first_time = false;
        }
        
        if(counter >= claculated_times){
            when_done = true;
            return 0;
        }

        block_sequence->execute(robot);
        double t = block_sequence->get_time_left();
        block_sequence->set_time_left(0);

        if (block_sequence->get_current_block() == nullptr) {
            counter++;
            //cout << "Counter: " << counter << endl;
            block_sequence->reset(robot);
            //cout << "Counter: " << counter << endl;
        }

        return t;
    }

    void finish(Robot& robot) override {
        first_time = true;
        counter = 0;
        when_done = false;
    }

    bool done(Robot& robot) override {
        return when_done;
    }
};

#endif