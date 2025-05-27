#ifndef DISPLAY_TEXT_HPP
#define DISPLAY_TEXT_HPP

#include "../common_block_includes.hpp"

using namespace std;

class DisplayText : public Block {
    Block* text;
    int letter_counter = 0;
    int word_counter = 0;
public:
    DisplayText(Block* text) : Block("Display", "DisplayText"), text(text) {}
    double execute(Robot& robot) override { // TODO: provjeri jel sve okej
        string str_text = text->executeString(robot);

        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                robot.pixel_display[i][j] = 0;
            }
        }

        if (word_counter >= text->executeString(robot).length()) {
            return 0;
        }

        if(str_text.length() == 1){
            for (int i = 0; i < 5; ++i) {
                for (int j = 0; j < 5; ++j) {
                    robot.pixel_display[i][j] = letter_matrices.at(str_text[0]).at(i).at(j) * robot.pixel_display_brightness;
                }
            }
            word_counter++;
            return 0.5;
        }

        else{
            for (int i = 0; i < 5; ++i) {
                for (int j = 0; j < 5; ++j) {
                    if(j == 4){
                        robot.pixel_display[i][j] = letter_matrices.at(str_text[word_counter]).at(i).at(letter_counter) * robot.pixel_display_brightness;
                    }
                    else robot.pixel_display[i][j] = robot.pixel_display[i][j+1];
                }
            }
            letter_counter++;	
            if (letter_counter >= 5) {
                letter_counter = 0;
                word_counter++;
            }
            robot.is_permanent_display = false;
            return 0.1067;
        }
    }

    bool done(Robot& robot) override {
        if (word_counter >= text->executeString(robot).length()) {
            return true;
        } else {
            return false;
        }
    }

    void finish(Robot& robot) override {
        word_counter = 0;
        letter_counter = 0;
    }

    void deal_with_interference(Robot& robot, BlockSequence* sequence) override {
        Block* block = sequence->get_current_block();
        if (block->name == "DisplayImageForTime" || block->name == "DisplayText") {
            block->finish(robot);
            sequence->set_time_left(0);
        }
    }
};

#endif