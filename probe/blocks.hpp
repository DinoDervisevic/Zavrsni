#ifndef BLOCKS_H
#define BLOCKS_H

#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <algorithm>
#include <random>
#include "json.hpp"
#include "robot.hpp"
#include "functions.hpp"

using namespace std;

using json = nlohmann::json;



class Block {
    public:
        Block* next = nullptr;
        Block* parent = nullptr;
        string type;
        string name;
        
        Block(const string& type, string name) : type(type), name(name) {}
    
        // changes the state of the robot and returns the number of seconds it took to execute the block (0 in case on instantaneus blocks such as speed change)
        virtual int execute(Robot& robot) = 0; 
        virtual ~Block() = default;
        bool done() {
            return true;
        }
};

class BlockSequence {
    Block* current_block;
    Block* starting_block;
    double time_left;
    bool is_running = false;
public:
    BlockSequence(Block* first_block) : current_block(first_block), starting_block(first_block),  time_left(0) {}

    void execute(Robot& robot) {
        if(time_left != 0){
            time_left -= robot.discrete_time_interval;
            time_left = max(time_left, 0.0);
        }
        while(time_left == 0 || current_block != nullptr) {
            time_left += current_block->execute(robot);
            if(current_block->done()) current_block = current_block->next;
        }
    }

    void reset() {
        current_block = starting_block;
        time_left = 0;
    }

    double get_time_left() {
        return time_left;
    }

    void set_time_left(int amount){
        time_left = amount;
    }

    Block* get_current_block() const {
        return current_block;
    }

    Block* get_starting_block() const {
        return starting_block;
    }

    void set_is_running(bool is_running) {
        this->is_running = is_running;
    }

    bool get_is_running() {
        return is_running;
    }
};




//-------------EVENT BLOCKS-------------------
// Since we know all events last 0 seconds, the return value tells us wether the condition is true or false (that is wether we need to start the block)
class WhenProgramStarts : public Block {
public:
    WhenProgramStarts() : Block("Event", "WhenProgramStarts") {}

    int execute(Robot& robot) override {
        return 0;
    }
};
//--------------------------------------------

//-------------OPERATOR BLOCKS----------------
class PickRandom : public Block {
    Block* from;
    Block* to;
public:
    PickRandom(Block* from, Block* to) : Block("Operator", "PickRandom"), from(from), to(to) {}

    int execute(Robot& robot) override {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(from->execute(robot), to->execute(robot));
        return dis(gen);
    }
};

class Add : public Block {
    Block* value1;
    Block* value2;
public:
    Add(Block* value1, Block* value2) : Block("Operator", "Add"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return value1->execute(robot) + value2->execute(robot);
    }
};

class Subtract : public Block {
    Block* value1;
    Block* value2;
public:
    Subtract(Block* value1, Block* value2) : Block("Operator", "Subtract"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return value1->execute(robot) - value2->execute(robot);
    }
};

class Multiply : public Block {
    Block* value1;
    Block* value2;
public:
    Multiply(Block* value1, Block* value2) : Block("Operator", "Multiply"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return value1->execute(robot) * value2->execute(robot);
    }
};

class Divide : public Block {
    Block* value1;
    Block* value2;
public:
    Divide(Block* value1, Block* value2) : Block("Operator", "Divide"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return value1->execute(robot) / value2->execute(robot);
    }
};

//Some of these return a binary value, and some a number or string, depending on the context in which they are used
class LessThan : public Block {
    Block* value1;
    Block* value2;
public:
    LessThan(Block* value1, Block* value2) : Block("Operator", "LessThan"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return value1->execute(robot) < value2->execute(robot);
    }
};

class Equals : public Block {
    Block* value1;
    Block* value2;
public:
    Equals(Block* value1, Block* value2) : Block("Operator", "Equals"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return value1->execute(robot) == value2->execute(robot);
    }
};

class GreaterThan : public Block {
    Block* value1;
    Block* value2;
public:
    GreaterThan(Block* value1, Block* value2) : Block("Operator", "GreaterThan"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return value1->execute(robot) > value2->execute(robot);
    }
};

class And : public Block {
    Block* value1;
    Block* value2;
public:
    And(Block* value1, Block* value2) : Block("Operator", "And"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return value1->execute(robot) && value2->execute(robot);
    }
};

class Or : public Block {
    Block* value1;
    Block* value2;
public:
    Or(Block* value1, Block* value2) : Block("Operator", "Or"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return value1->execute(robot) || value2->execute(robot);
    }
};

class Not : public Block {
    Block* value;
public:
    Not(Block* value) : Block("Operator", "Not"), value(value) {}

    int execute(Robot& robot) override {
        return !value->execute(robot);
    }
};

class IsInBetween : public Block {
    Block* value1;
    Block* value2;
    Block* value3;
public:
    IsInBetween(Block* value1, Block* value2, Block* value3) : Block("Operator", "IsInBetween"), value1(value1), value2(value2), value3(value3) {}

    int execute(Robot& robot) override {
        return value1->execute(robot) < value2->execute(robot) && value2->execute(robot) < value3->execute(robot);
    }
};

class Join : public Block {
    Block* value1;
    Block* value2;
public:
    Join(Block* value1, Block* value2) : Block("Operator", "Join"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return 0; // TODO: implement string joining
    }
};

class LetterOf : public Block {
    Block* value1;
    Block* value2;
public:
    LetterOf(Block* value1, Block* value2) : Block("Operator", "LetterOf"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return 0; // TODO: implement string letter extraction
    }
};

class LengthOf : public Block {
    Block* value;
public:
    LengthOf(Block* value) : Block("Operator", "LengthOf"), value(value) {}

    int execute(Robot& robot) override {
        return 0; // TODO: implement string length calculation
    }
};

class Contains : public Block {
    Block* value1;
    Block* value2;
public:
    Contains(Block* value1, Block* value2) : Block("Operator", "Contains"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return 0; // TODO: implement string contains check
    }
};

class Modulus : public Block {
    Block* value1;
    Block* value2;
public:
    Modulus(Block* value1, Block* value2) : Block("Operator", "Modulus"), value1(value1), value2(value2) {}

    int execute(Robot& robot) override {
        return value1->execute(robot) % value2->execute(robot);
    }
};

class Round : public Block {
    Block* value;
public:
    Round(Block* value) : Block("Operator", "Round"), value(value) {}

    int execute(Robot& robot) override {
        return round(value->execute(robot));
    }
};

class MathOp : public Block {
    Block* value;
    string function_name;
public:
    MathOp(Block* value, string function_name) : Block("Operator", "MathOp"), value(value), function_name(function_name) {}

    int execute(Robot& robot) override { // TODO: add rest of them
        if(function_name == "sin"){
            return sin(value->execute(robot));
        } else if(function_name == "cos"){
            return cos(value->execute(robot));
        } else if(function_name == "tan"){
            return tan(value->execute(robot));
        } else if(function_name == "asin"){
            return asin(value->execute(robot));
        } else if(function_name == "acos"){
            return acos(value->execute(robot));
        } else if(function_name == "atan"){
            return atan(value->execute(robot));
        } else if(function_name == "ln"){
            return log(value->execute(robot));
        } else if(function_name == "log"){
            return log10(value->execute(robot));
        }
        return 0;
    }
};
//--------------------------------------------
//-----------BLANK BLOCKS---------------------
class BlankBlockInt : public Block {
    int value;
public:
    BlankBlockInt(int value) : Block("Blank", "BlankBlockInt"), value(value) {}

    int execute(Robot& robot) override {
        return value;
    }
};

class BlankBlockDouble : public Block {
    double value;
public:
    BlankBlockDouble(double value) : Block("Blank", "BlankBlockDouble"), value(value) {}

    int execute(Robot& robot) override {
        return value;
    }
};

class BlankBlockString : public Block {
    string value;
public:
    BlankBlockString(string value) : Block("Blank", "BlankBlockString"), value(value) {}

    int execute(Robot& robot) override {
        return 0;
    }
};
//--------------------------------------------

//-------------MOVEMENT BLOCKS----------------
class Move : public Block { // TODO :  on this and every other move block, change speed of motors, not wheels (when you get the robot that is)
    bool forward;
    Block* value;
    string unit;
public:
    Move(bool forward, Block* value, string unit) : Block("Move", "Move"), forward(forward), value(value), unit(unit) {}

    int execute(Robot& robot) override {
        if(forward){
            robot.v1 = robot.movement_speed;
            robot.v2 = robot.movement_speed;
        } else {
            robot.v1 = -robot.movement_speed;
            robot.v2 = -robot.movement_speed;
        }
        cout << "Moving for " << value->execute(robot) << " seconds" << endl;
        return convert_to_seconds(robot, unit, value->execute(robot));
    }
    
};

class StartMove : public Block {
    bool forward;

public:
    StartMove(bool forward) : Block("Move", "StartMove"), forward(forward) {}

    int execute(Robot& robot) override {
        if(forward){
            robot.v1 = robot.movement_speed;
            robot.v2 = robot.movement_speed;
        } else {
            robot.v1 = -robot.movement_speed;
            robot.v2 = -robot.movement_speed;
        }
        return -1;
    }
};

class Steer : public Block {
    Block* direction;
    Block* value;
    string unit;

public:
    Steer(Block* direction, Block* value, string unit) : Block("Move", "Steer"), direction(direction), value(value), unit(unit) {}

    int execute(Robot& robot) override { // TODO : provjeri jel ovo dobro
        robot.v1 = robot.movement_speed * min(1 - direction->execute(robot)/100, 1);
        robot.v2 = robot.movement_speed * min(1 + direction->execute(robot)/100, 1);

        return convert_to_seconds(robot, unit, value->execute(robot));
    }
};

class StartSteer : public Block {
    Block* direction;

public:
    StartSteer(Block* direction) : Block("Move", "StartSteer"), direction(direction) {}

    int execute(Robot& robot) override {
        robot.v1 = robot.movement_speed * min(1 - direction->execute(robot)/100, 1);
        robot.v2 = robot.movement_speed * min(1 + direction->execute(robot)/100, 1);

        return -1;
    }
};

class StopMoving : public Block {
public:
    StopMoving() : Block("Move", "StopMoving") {}

    int execute(Robot& robot) override {
        robot.v1 = 0.0;
        robot.v2 = 0.0;
        return 0;
    }
};

class SetMovementSpeed : public Block {
    Block* speed;
public:
    SetMovementSpeed(Block* speed) : Block("Move", "SetMovementSpeed"), speed(speed) {}

    int execute(Robot& robot) override {
        robot.movement_speed = speed->execute(robot);
        return 0;
    }
};

class SetMovementPair : public Block { // TODO : change so this only marks these two ports as wheels, not the wheels as being on these two ports
    Block* pair;
public:
    SetMovementPair(Block* pair) : Block("Move", "SetMovementPair"), pair(pair) {}
    int execute(Robot& robot) override { //TODO

        return 0;
    }
};

class SetMotorRotation : public Block { // TODO
    string unit;
    Block* value;
public:
    SetMotorRotation(string unit, Block* value) : Block("Move", "SetMotorRotation"), unit(unit), value(value) {}

    int execute(Robot& robot) override { // TODO
        return 0;
    }
};
//--------------------------------------------

//-------------DISPLAY BLOCKS-----------------
class DisplayImageForTime : public Block {
    string image;
    double time;
public:
    DisplayImageForTime(string image, double time) : Block("Display", "DisplayImageForTime"), image(image), time(time) {}

    int execute(Robot& robot) override {
        for(int i = 0; i < 5; ++i){
            for(int j = 0; j < 5; ++j){
                robot.pixel_display[i][j] = image[i*5 + j] * 100 * 10 / 9;
            }
        }
        return time;
    }
};

class DisplayImage : public Block {
    string image;
public:
    DisplayImage(string image) : Block("Display", "DisplayImage"), image(image) {}

    int execute(Robot& robot) override {
        for(int i = 0; i < 5; ++i){
            for(int j = 0; j < 5; ++j){
                robot.pixel_display[i][j] = image[i*5 + j] * 100 * 10 / 9;
            }
        }
        return -1;
    }
};

class DisplayText : public Block { // TODO
    string text;
public:
    DisplayText(string text) : Block("Display", "DisplayText"), text(text) {}
    int execute(Robot& robot) override { // TODO
        return 0;
    }
};

class DisplayOff : public Block {
public:
    DisplayOff() : Block("Display", "DisplayOff") {}

    int execute(Robot& robot) override {
        for(int i = 0; i < 5; ++i){
            for(int j = 0; j < 5; ++j){
                robot.pixel_display[i][j] = 0;
            }
        }
        return 0;
    }
};

class SetPixelbrightness : public Block { // TODO
    double brightness;
public:
    SetPixelbrightness(double brightness) : Block("Display", "SetPixelbrightness"), brightness(brightness) {}

    int execute(Robot& robot) override { // TODO
        
        return 0;
    }
};

class SetPixel : public Block {
    int x, y;
    double brightness;
public:
    SetPixel(int x, int y, double brightness) : Block("Display", "SetPixel"), x(x), y(y), brightness(brightness) {}

    int execute(Robot& robot) override {
        robot.pixel_display[x][y] = brightness;
        return -1;
    }
};

class DisplayRotate : public Block { // TODO
    string direction;
public:
    DisplayRotate(string direction) : Block("Display", "DisplayRotate"), direction(direction) {}
    int execute(Robot& robot) override { // TODO
        return 0;
    }
};

class DisplaySetorientation : public Block { // TODO
    string orientation;
public:
    DisplaySetorientation(string orientation) : Block("Display", "DisplaySetorientation"), orientation(orientation) {}
    int execute(Robot& robot) override { // TODO
        return 0;
    }
};

class CenterButtonlight : public Block {
    string color;

public:
    CenterButtonlight(string color) : Block("Display", "CenterButtonlight"), color(color) {}

    int execute(Robot& robot) override {
        robot.button_color_state = color;
        return -1;
    }
};

class UltrasonicSensorLight : public Block { // TODO : finish when i know what to do w ports
    string color;
    string port;

public:
    UltrasonicSensorLight(string color, string port) : Block("Display", "UltrasonicSensorLight"), color(color), port(port) {}

    int execute(Robot& robot) override {
        robot.sensor_color = color;
        return -1;
    }
};
//--------------------------------------------


//-------------SOUND BLOCKS-------------------
class PlayUntilDone : public Block { // TODO : finish
    string sound_name;
    string sound_location;
public:
    PlayUntilDone(string sound, string sound_location) : Block("Sound", "PlayUntilDone"), sound_name(sound_name), sound_location(sound_location) {};
    int execute(Robot& robot) override { // TODO
        return 0;
    }
};

class Play : public Block { // TODO : finish
    string sound_name;
    string sound_location;
public:
    Play(string sound, string sound_location) : Block("Sound", "Play"), sound_name(sound_name), sound_location(sound_location) {};
    int execute(Robot& robot) override { // TODO
        return 0;
    }
};

class BeepForTime : public Block {
    double time;
    double frequency;
public:
    BeepForTime(double time, double frequency) : Block("Sound", "BeepForTime"), time(time), frequency(frequency) {}

    int execute(Robot& robot) override {
        robot.sound_state = "beep" + to_string(frequency);
        return time;
    }
};

class Beep : public Block {
    double frequency;
public:
    Beep(double frequency) : Block("Sound", "Beep"), frequency(frequency) {}

    int execute(Robot& robot) override {
        robot.sound_state = "beep" + to_string(frequency);
        return -1;
    }
};

class StopSound : public Block {
public:
    StopSound() : Block("Sound", "StopSound") {}

    int execute(Robot& robot) override {
        robot.sound_state = "";
        return 0;
    }
};

class ChangeEffectBy : public Block { // TODO
    string effect;
    double value;
public:
    ChangeEffectBy(string effect, double value) : Block("Sound", "ChangeEffectBy"), effect(effect), value(value) {}

    int execute(Robot& robot) override { // TODO
        return 0;
    }
};

class SetEffectTo : public Block { // TODO
    string effect;
    double value;
public:
    SetEffectTo(string effect, double value) : Block("Sound", "SetEffectTo"), effect(effect), value(value) {}

    int execute(Robot& robot) override { // TODO
        return 0;
    }
};

class ClearEffects : public Block { // TODO
public:
    ClearEffects() : Block("Sound", "ClearEffects") {}

    int execute(Robot& robot) override {
        return 0;
    }
};

class ChangeVolumeBy : public Block {
    double value;
public:
    ChangeVolumeBy(double value) : Block("Sound", "ChangeVolumeBy"), value(value) {}

    int execute(Robot& robot) override {
        robot.volume += value;
        return 0;
    }
};

class SetVolumeTo : public Block {
    double value;
public:
    SetVolumeTo(double value) : Block("Sound", "SetVolumeTo"), value(value) {}

    int execute(Robot& robot) override {
        robot.volume = value;
        return 0;
    }
};
//--------------------------------------------
//-----------MOTOR BLOCKS--------------------
class MotorTurnForDirection : public Block {
    string port;
    Block* speed;
    string direction;
    string unit;
public:
    MotorTurnForDirection(string port, Block* speed, string direction, string unit) : Block("Motor", "MotorTurnForDirection"), port(port), speed(speed), direction(direction), unit(unit) {}

    int execute(Robot& robot) override { //TODO
        return 0;
    }
};

class MotorStop : public Block {
public:
    MotorStop() : Block("Motor", "MotorStop") {}
    int execute(Robot& robot) override {
        robot.v1 = 0.0;
        robot.v2 = 0.0;
        return 0;
    }
};
//--------------------------------------------
//-----------CONTROL BLOCKS-------------------
class Wait : public Block {
    Block* time;
public:
    Wait(Block* time) : Block("Control", "Wait"), time(time) {}

    int execute(Robot& robot) override {
        return time->execute(robot);
    }
};

class Repeat : public Block {
    Block* times; // TODO: check if this needs to be calculated only once or every time
    BlockSequence* block_sequence;
    int counter;
    bool done = false;
public:
    Repeat(Block* times, BlockSequence* block_sequence) : Block("Control", "Repeat"), times(times), block_sequence(block_sequence), counter(0) {}

    int execute(Robot& robot) override {
        if (block_sequence == nullptr) {
            return 0;
        }
        if (block_sequence->get_current_block()->next == nullptr) {
            counter++;
            block_sequence->reset();
        }
        if(counter == times->execute(robot)){
            done = true;
            return 0;
        }
        block_sequence->execute(robot);
        int t = block_sequence->get_time_left();
        block_sequence->set_time_left(0);
        return t;
    }

    bool done() {
        return done;
    }
};

class Forever : public Block {
    BlockSequence* block_sequence;
public:
    Forever(BlockSequence* block_sequence) : Block("Control", "Forever"), block_sequence(block_sequence) {}

    int execute(Robot& robot) override {
        if (block_sequence == nullptr) {
            return 0;
        }
        if (block_sequence->get_current_block()->next == nullptr) block_sequence->reset();

        block_sequence->execute(robot);
        int t = block_sequence->get_time_left();
        block_sequence->set_time_left(0);
        return t;
    }

    bool done() {
        return false;
    }
};

class If : public Block {
    BlockSequence* block_sequence;
    bool condition_checked, done = false;
    Block* condition;
public:
    If(BlockSequence* block_sequence, Block* condition) : Block("Control", "If"), block_sequence(block_sequence), condition(condition) {}

    int execute(Robot& robot) override {
        if((condition->execute(robot) && !condition_checked) || condition_checked){
            if(block_sequence->get_current_block()->next == nullptr){
                done = true;
                block_sequence->reset();
            }
            condition_checked = true;
            block_sequence->execute(robot);
            int t = block_sequence->get_time_left();
            block_sequence->set_time_left(0);
            return t;
        }
        else{
            done = true;
        }
        return 0;
    }

    bool done() {
        return done;
    }
};

class IfElse : public Block {
    BlockSequence* block_sequence1;
    BlockSequence* block_sequence2;
    bool condition_checked, else_checked, done = false;
    Block* condition;
public:
    IfElse(BlockSequence* block_sequence1, BlockSequence* block_sequence2, Block* condition) : Block("Control", "IfElse"), block_sequence1(block_sequence1), block_sequence2(block_sequence2), condition(condition) {}

    int execute(Robot& robot) override {
        int t = 0;
        if((condition->execute(robot) && !condition_checked && !else_checked) || condition_checked){
            condition_checked = true;
            block_sequence1->execute(robot);
            t = block_sequence1->get_time_left();
            block_sequence1->set_time_left(0);
        } else {
            else_checked = true;
            block_sequence2->execute(robot);
            t = block_sequence2->get_time_left();
            block_sequence2->set_time_left(0);
        }
        if((block_sequence1->get_current_block()->next == nullptr && condition_checked) || (block_sequence2->get_current_block()->next == nullptr && else_checked)){
            done = true;
            block_sequence1->reset();
            block_sequence2->reset();
        }
        return t;
    }

    bool done() {
        return done;
    }
};

class WaitUntil : public Block {
    Block* condition;
    bool done = false;
public:
    WaitUntil(Block* condition) : Block("Control", "WaitUntil"), condition(condition) {}

    int execute(Robot& robot) override {
        if(condition->execute(robot) == 1){
            done = true;
            return 0;
        }
        return -1;
    }

    bool done() {
        return done;
    }
};

class RepeatUntil : public Block {
    Block* condition;
    BlockSequence* block_sequence;
    bool done = false;
public:
    RepeatUntil(Block* condition, BlockSequence* block_sequence) : Block("Control", "RepeatUntil"), condition(condition), block_sequence(block_sequence) {}

    int execute(Robot& robot) override {
        if (block_sequence == nullptr) {
            return 0;
        }
        if (block_sequence->get_current_block()->next == nullptr) {
            block_sequence->reset();
        }
        if(block_sequence->get_current_block() == block_sequence->get_starting_block() && condition->execute(robot) == 1){
            done = true;
            return 0;
        }
        block_sequence->execute(robot);
        int t = block_sequence->get_time_left();
        block_sequence->set_time_left(0);
        return t;
    }

    bool done() {
        return done;
    }
};

//--------------------------------------------
//-----------END OF BLOCKS--------------------



//-----------FUNCTION MAP---------------------

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;

FunctionMap createFunctionMap() {
    FunctionMap functionMap;

    // Event blocks
    functionMap["flipperevents_whenProgramStarts"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<WhenProgramStarts>();
    };
    //--------------------------------------------

    // Operator blocks
    functionMap["operator_random"] = [&functionMap](const json& json_object, const string& name) {
        Block* from;
        Block* to;
        if(json_object[name]["inputs"]["FROM"][0] == 1){
            from = new BlankBlockInt(stoi(json_object[name]["inputs"]["FROM"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["FROM"][1];
            from = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }

        if(json_object[name]["inputs"]["TO"][0] == 1){
            to = new BlankBlockInt(stoi(json_object[name]["inputs"]["TO"][1][1].get<string>()));
        } else {
            string to_name = json_object[name]["inputs"]["TO"][1];
            to = functionMap[json_object[to_name]["opcode"]](json_object, to_name).release();
        }
        return make_unique<PickRandom>(from, to);
    };

    functionMap["operator_add"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Add>(value1, value2);
    };

    functionMap["operator_subtract"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Subtract>(value1, value2);
    };

    functionMap["operator_multiply"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Multiply>(value1, value2);
    };

    functionMap["operator_divide"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Divide>(value1, value2);
    };

    functionMap["operator_lt"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["OPERAND1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["OPERAND1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["OPERAND2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["OPERAND2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<LessThan>(value1, value2);
    };

    functionMap["operator_equals"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["OPERAND1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["OPERAND1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["OPERAND2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["OPERAND2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Equals>(value1, value2);
    };

    functionMap["operator_gt"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["OPERAND1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["OPERAND1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["OPERAND2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["OPERAND2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<GreaterThan>(value1, value2);
    };

    functionMap["operator_and"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if (json_object[name]["inputs"].contains("OPERAND1")) {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        } else {
            cerr << "Error: Operator AND is missing input one!" << endl;
        }

        if (json_object[name]["inputs"].contains("OPERAND2")) {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        } else {
            cerr << "Error: Operator AND is missing input two!" << endl;
        }
        return make_unique<And>(value1, value2);
    };

    functionMap["operator_or"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if (json_object[name]["inputs"].contains("OPERAND1")) {
            string value1_name = json_object[name]["inputs"]["OPERAND1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        } else {
            cerr << "Error: Operator OR is missing input one!" << endl;
        }

        if (json_object[name]["inputs"].contains("OPERAND2")) {
            string value2_name = json_object[name]["inputs"]["OPERAND2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        } else {
            cerr << "Error: Operator OR is missing input two!" << endl;
        }
        return make_unique<Or>(value1, value2);
    };

    functionMap["operator_not"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        if (json_object[name]["inputs"].contains("OPERAND")) {
            string value1_name = json_object[name]["inputs"]["OPERAND"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        } else {
            cerr << "Error: Operator NOT is missing input!" << endl;
        }
        return make_unique<Not>(value1);
    };

    functionMap["flipperoperator_isInBetween"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        Block* value3;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["VALUE"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["LOW"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["LOW"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["LOW"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }

        if(json_object[name]["inputs"]["HIGH"][0] == 1){
            value3 = new BlankBlockDouble(stod(json_object[name]["inputs"]["HIGH"][1][1].get<string>()));
        } else {
            string value3_name = json_object[name]["inputs"]["HIGH"][1];
            value3 = functionMap[json_object[value3_name]["opcode"]](json_object, value3_name).release();
        }
        return make_unique<IsInBetween>(value1, value2, value3);
    };

    functionMap["operator_join"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["STRING1"][0] == 1){
            value1 = new BlankBlockString(json_object[name]["inputs"]["STRING1"][1][1].get<string>());
        } else {
            string value1_name = json_object[name]["inputs"]["STRING1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["STRING2"][0] == 1){
            value2 = new BlankBlockString(json_object[name]["inputs"]["STRING2"][1][1].get<string>());
        } else {
            string value2_name = json_object[name]["inputs"]["STRING2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Join>(value1, value2);
    };

    functionMap["operator_letterof"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["STRING"][0] == 1){
            value2 = new BlankBlockString(json_object[name]["inputs"]["STRING"][1][1].get<string>());
        } else {
            string value1_name = json_object[name]["inputs"]["STRING"][1];
            value2 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["LETTER"][0] == 1){
            value1 = new BlankBlockInt(stoi(json_object[name]["inputs"]["LETTER"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["LETTER"][1];
            value1 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<LetterOf>(value1, value2);
    };

    functionMap["operator_length"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        if(json_object[name]["inputs"]["STRING"][0] == 1){
            value1 = new BlankBlockString(json_object[name]["inputs"]["STRING"][1][1].get<string>());
        } else {
            string value1_name = json_object[name]["inputs"]["STRING"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }
        return make_unique<LengthOf>(value1);
    };

    functionMap["operator_contains"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["STRING1"][0] == 1){
            value1 = new BlankBlockString(json_object[name]["inputs"]["STRING1"][1][1].get<string>());
        } else {
            string value1_name = json_object[name]["inputs"]["STRING1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["STRING2"][0] == 1){
            value2 = new BlankBlockString(json_object[name]["inputs"]["STRING2"][1][1].get<string>());
        } else {
            string value2_name = json_object[name]["inputs"]["STRING2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Contains>(value1, value2);
    };

    functionMap["operator_mod"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        Block* value2;
        if(json_object[name]["inputs"]["NUM1"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM1"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM1"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        if(json_object[name]["inputs"]["NUM2"][0] == 1){
            value2 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM2"][1][1].get<string>()));
        } else {
            string value2_name = json_object[name]["inputs"]["NUM2"][1];
            value2 = functionMap[json_object[value2_name]["opcode"]](json_object, value2_name).release();
        }
        return make_unique<Modulus>(value1, value2);
    };

    functionMap["operator_round"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        if(json_object[name]["inputs"]["NUM"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }
        return make_unique<Round>(value1);
    };

    functionMap["operator_mathop"] = [&functionMap](const json& json_object, const string& name) {
        Block* value1;
        if(json_object[name]["inputs"]["NUM"][0] == 1){
            value1 = new BlankBlockDouble(stod(json_object[name]["inputs"]["NUM"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["NUM"][1];
            value1 = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }
        return make_unique<MathOp>(value1, json_object[name]["fields"]["OPERATOR"][0]);
    };
    //--------------------------------------------

    // Movement blocks
    functionMap["flippermove_move"] = [&functionMap](const json& json_object, const string& name) {
        bool forward;
        int args = json_object[name]["inputs"]["DIRECTION"][1];
        if(args != 1){
            forward = true;
        }
        else{
            string direction_name = json_object[name]["inputs"]["DIRECTION"][1];
            string fwd = json_object[direction_name]["fields"]["field_flippermove_custom-icon-direction"][0];
            
            if(fwd == "forward") {
                forward = true;
            } else {
                forward = false;
            }
        }
        
        string unit = json_object[name]["fields"]["UNIT"][0];

        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }

        return make_unique<Move>(forward, value, unit);
    };

    functionMap["flippermove_startMove"] = [&functionMap](const json& json_object, const string& name) {
        bool forward;
        int args = json_object[name]["inputs"]["DIRECTION"][1];
        if(args != 1){
            forward = true;
        }
        else{
            string direction_name = json_object[name]["inputs"]["DIRECTION"][1];
            string fwd = json_object[direction_name]["fields"]["field_flippermove_custom-icon-direction"][0];
            
            if(fwd == "forward") {
                forward = true;
            } else {
                forward = false;
            }
        }
        return make_unique<StartMove>(forward);
    };

    functionMap["flippermove_rotation-wheel"] = [&functionMap](const json& json_object, const string& name) {
        string direction = json_object[name]["fields"]["field_flippermove_rotation-wheel"][0];
        int value = stoi(direction.substr(0, direction.find(" ")));
        return make_unique<BlankBlockInt>(value);
    };

    functionMap["flippermove_steer"] = [&functionMap](const json& json_object, const string& name) {
        string direction_name = json_object[name]["inputs"]["STEERING"][1];
        Block* direction = functionMap[json_object[direction_name]["opcode"]](json_object, direction_name).release();
        double value = stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>());
        string unit = json_object[name]["fields"]["UNIT"][0];
        return make_unique<Steer>(direction, value, unit);
    };

    functionMap["flippermove_startSteer"] = [&functionMap](const json& json_object, const string& name) {
        string direction_name = json_object[name]["inputs"]["STEERING"][1];
        Block* direction = functionMap[json_object[direction_name]["opcode"]](json_object, direction_name).release();
        return make_unique<StartSteer>(direction);
    };

    functionMap["flippermove_stopMove"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<StopMoving>();
    };

    functionMap["flippermove_movementSpeed"] = [&functionMap](const json& json_object, const string& name) {
        Block* speed;
        if(json_object[name]["inputs"]["SPEED"][0] == 1){
            speed = new BlankBlockDouble(stod(json_object[name]["inputs"]["SPEED"][1][1].get<string>()));
        } else {
            string speed_name = json_object[name]["inputs"]["SPEED"][1];
            speed = functionMap[json_object[speed_name]["opcode"]](json_object, speed_name).release();
        }
        return make_unique<SetMovementSpeed>(speed);
    };

    functionMap["flippermove_setMovementPair"] = [&functionMap](const json& json_object, const string& name) {
        string pair_name = json_object[name]["inputs"]["PAIR"][1];
        Block* pair = functionMap[json_object[pair_name]["opcode"]](json_object, pair_name).release();
        return make_unique<SetMovementPair>(pair);
    };

    functionMap["flippermove_setDistance"] = [&functionMap](const json& json_object, const string& name) {
        string unit = json_object[name]["fields"]["UNIT"][0];
        Block* distance;
        if(json_object[name]["inputs"]["DISTANCE"][0] == 1){
            distance = new BlankBlockDouble(stod(json_object[name]["inputs"]["DISTANCE"][1][1].get<string>()));
        } else {
            string distance_name = json_object[name]["inputs"]["DISTANCE"][1];
            distance = functionMap[json_object[distance_name]["opcode"]](json_object, distance_name).release();
        }
        return make_unique<SetMotorRotation>(unit, distance);
    };
    //--------------------------------------------
    // Display blocks
    functionMap["flipperlight_lightDisplayImageOnForTime"] = [&functionMap](const json& json_object, const string& name) {
        string matrix_name = json_object[name]["inputs"]["MATRIX"][1];
        string image = json_object[matrix_name]["fields"]["field_flipperlight_matrix-5x5-brightness-image"][0];
        double time = stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>());
        return make_unique<DisplayImageForTime>(image, time);
    };

    functionMap["flipperlight_lightDisplayImageOn"] = [&functionMap](const json& json_object, const string& name) {
        string matrix_name = json_object[name]["inputs"]["MATRIX"][1];
        string image = json_object[matrix_name]["fields"]["field_flipperlight_matrix-5x5-brightness-image"][0];
        return make_unique<DisplayImage>(image);
    };

    functionMap["flipperlight_lightDisplayText"] = [&functionMap](const json& json_object, const string& name) {
        string text = json_object[name]["inputs"]["TEXT"][1][1].get<string>();
        return make_unique<DisplayText>(text);
    };

    functionMap["flipperlight_lightDisplayOff"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<DisplayOff>();
    };

    functionMap["flipperlight_lightDisplaySetBrightness"] = [&functionMap](const json& json_object, const string& name) {
        double brightness = stod(json_object[name]["inputs"]["BRIGHTNESS"][1][1].get<string>());
        return make_unique<SetPixelbrightness>(brightness);
    };

    functionMap["flipperlight_lightDisplaySetPixel"] = [&functionMap](const json& json_object, const string& name) {
        string x_name = json_object[name]["inputs"]["X"][1];
        string y_name = json_object[name]["inputs"]["Y"][1];
        int x = stoi(json_object[x_name]["fields"]["field_flipperlight_matrix-pixel-index"][0].get<string>());
        int y = stoi(json_object[y_name]["fields"]["field_flipperlight_matrix-pixel-index"][0].get<string>());
        double brightness = stod(json_object[name]["inputs"]["BRIGHTNESS"][1][1].get<string>());
        return make_unique<SetPixel>(x, y, brightness);
    };

    functionMap["flipperlight_lightDisplayRotate"] = [&functionMap](const json& json_object, const string& name) {
        string direction_name = json_object[name]["inputs"]["DIRECTION"][1];
        string direction = json_object[direction_name]["fields"]["field_flipperlight_custom-icon-direction"][0].get<string>();
        return make_unique<DisplayRotate>(direction);
    };

    functionMap["flipperlight_lightDisplaySetOrientation"] = [&functionMap](const json& json_object, const string& name) {
        string orientation_name = json_object[name]["inputs"]["ORIENTATION"][1];
        string orientation = json_object[orientation_name]["fields"]["orientation"][0].get<string>();
        return make_unique<DisplaySetorientation>(orientation);
    };

    functionMap["flipperlight_centerButtonLight"] = [&functionMap](const json& json_object, const string& name) {
        string color_name = json_object[name]["inputs"]["COLOR"][1];
        string color = json_object[color_name]["fields"]["field_flipperlight_color-selector-vertical"][0].get<string>();
        return make_unique<CenterButtonlight>(color);
    };

    functionMap["flipperlight_ultrasonicLightUp"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        string value_name = json_object[name]["inputs"]["VALUE"][1];
        string color = json_object[value_name]["fields"]["field_flipperlight_color-selector-vertical"][0].get<string>();
        string port = json_object[port_name]["fields"]["field_flipperlight_led-selector"][0].get<string>();
        return make_unique<UltrasonicSensorLight>(color, port);
    };
    //--------------------------------------------
    // Sound blocks
    functionMap["flippersound_playSoundUntilDone"] = [&functionMap](const json& json_object, const string& name) {
        string sound_name = json_object[name]["inputs"]["SOUND"][1];
        string sound_string = json_object[sound_name]["fields"]["field_flippersound_sound-selector"][0].get<string>();
        json sound_json = json::parse(sound_string);
        string sound = sound_json["name"].get<string>();
        string sound_location = sound_json["location"].get<string>();

        return make_unique<PlayUntilDone>(sound, sound_location);
    };

    functionMap["flippersound_playSound"] = [&functionMap](const json& json_object, const string& name) {
        string sound_name_name = json_object[name]["inputs"]["SOUND"][1];
        string sound_string = json_object[sound_name_name]["fields"]["field_flippersound_sound-selector"][0].get<string>();
        json sound_json = json::parse(sound_string);
        string sound_name = sound_json["name"].get<string>();
        string sound_location = sound_json["location"].get<string>();

        return make_unique<Play>(sound_name, sound_location);
    };

    functionMap["flippersound_beepForTime"] = [&functionMap](const json& json_object, const string& name) {
        double time = stod(json_object[name]["inputs"]["DURATION"][1][1].get<string>());
        string frequency_name = json_object[name]["inputs"]["NOTE"][1];
        double frequency = stod(json_object[frequency_name]["fields"]["field_flippersound_custom-piano"][0].get<string>());
        return make_unique<BeepForTime>(time, frequency);
    };

    functionMap["flippersound_beep"] = [&functionMap](const json& json_object, const string& name) {
        string frequency_name = json_object[name]["inputs"]["NOTE"][1];
        double frequency = stod(json_object[frequency_name]["fields"]["field_flippersound_custom-piano"][0].get<string>());
        return make_unique<Beep>(frequency);
    };

    functionMap["flippersound_stopSound"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<StopSound>();
    };

    functionMap["sound_changeeffectby"] = [&functionMap](const json& json_object, const string& name) {
        string effect = json_object[name]["fields"]["EFFECT"][1].get<string>();
        double value = stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>());
        return make_unique<ChangeEffectBy>(effect, value);
    };

    functionMap["sound_seteffectto"] = [&functionMap](const json& json_object, const string& name) {
        string effect = json_object[name]["fields"]["EFFECT"][1].get<string>();
        double value = stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>());
        return make_unique<SetEffectTo>(effect, value);
    };

    functionMap["sound_cleareffects"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<ClearEffects>();
    };

    functionMap["sound_changevolumeby"] = [&functionMap](const json& json_object, const string& name) {
        double value = stod(json_object[name]["inputs"]["VOLUME"][1][1].get<string>());
        return make_unique<ChangeVolumeBy>(value);
    };

    functionMap["sound_setvolumeto"] = [&functionMap](const json& json_object, const string& name) {
        double value = stod(json_object[name]["inputs"]["VOLUME"][1][1].get<string>());
        return make_unique<SetVolumeTo>(value);
    };
    //--------------------------------------------

    //Motor blocks
    functionMap["flippermotor_motorTurnForDirection"] = [&functionMap](const json& json_object, const string& name) {
        string direction_name = json_object[name]["inputs"]["DIRECTION"][1];
        string direction = json_object[direction_name]["fields"]["field_flippermotor_custom-icon-direction"][0].get<string>();
        string port_name = json_object[name]["inputs"]["PORT"][1];
        string port = json_object[direction_name]["fields"]["field_flippermotor_multiple-port-selector"][0].get<string>();
        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();
        
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        return make_unique<MotorTurnForDirection>(port, value, direction, unit);
    };
    //---------------------------------------------

    // Control blocks
    functionMap["control_wait"] = [&functionMap](const json& json_object, const string& name) {
        Block* duration;
        if(json_object[name]["inputs"]["DURATION"][0] == 1){
            duration = new BlankBlockDouble(stod(json_object[name]["inputs"]["DURATION"][1][1].get<string>()));
        } else {
            string duration_name = json_object[name]["inputs"]["DURATION"][1];
            duration = functionMap[json_object[duration_name]["opcode"]](json_object, duration_name).release();
        }
        return make_unique<Wait>(duration);
    };

    functionMap["control_repeat"] = [&functionMap](const json& json_object, const string& name) {
        Block* times;
        if(json_object[name]["inputs"]["TIMES"][0] == 1){
            times = new BlankBlockInt(stoi(json_object[name]["inputs"]["TIMES"][1][1].get<string>()));
        } else {
            string times_name = json_object[name]["inputs"]["TIMES"][1];
            times = functionMap[json_object[times_name]["opcode"]](json_object, times_name).release();
        }
        BlockSequence* block_sequence = new BlockSequence(nullptr);
        return make_unique<Repeat>(times, block_sequence);
    };

    functionMap["control_forever"] = [&functionMap](const json& json_object, const string& name) {
        BlockSequence* block_sequence = new BlockSequence(nullptr);
        return make_unique<Forever>(block_sequence);
    };
    //--------------------------------------------
    // Miscelanious helper "blocks"
    functionMap["flippermove_movement-port-selector"] = [&functionMap](const json& json_object, const string& name) {
        string port = json_object[name]["fields"]["field_flippermove_multiple-port-selector"][0].get<string>();
        return make_unique<BlankBlockString>(port);
    };

    //TODO : add the rest of the blocks when i know what to do w conditions

    return functionMap;
}

#endif // BLOCKS_H