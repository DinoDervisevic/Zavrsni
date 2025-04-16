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
#include <cctype>
#include <cmath>
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
        virtual double execute(Robot& robot) = 0; 
        virtual void finish(Robot& robot){
            return;
        }
        virtual string executeString(Robot& robot) {
            return to_string(execute(robot));
        }
        virtual ~Block() = default;
        virtual bool done() {
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

BlockSequence* processBlock(const json& blocks, string key);


//-------------EVENT BLOCKS-------------------
// Since we know all events last 0 seconds, the return value tells us wether the condition is true or false (that is wether we need to start the block)
class WhenProgramStarts : public Block {
public:
    WhenProgramStarts() : Block("Event", "WhenProgramStarts") {}

    double execute(Robot& robot) override {
        return 1;
    }
};

class WhenColor : public Block {
    Block* port;
    Block* color;
public:
    WhenColor(Block* port, Block* color) : Block("Event", "WhenColor"), port(port), color(color) {}

    double execute(Robot& robot) override {
        if (robot.color_states.find(port->executeString(robot)) != robot.color_states.end() 
        && robot.color_states[port->executeString(robot)]->value == color->execute(robot)) {
            return 1;
        }
        else return 0;
    }
};

class WhenPressed : public Block {
    Block* port;
    string event;
public:
    WhenPressed(Block* port, string event) : Block("Event", "WhenPressed"), port(port), event(event) {}

    double execute(Robot& robot) override {
        if (robot.force_states.find(port->executeString(robot)) != robot.force_states.end()
        && calculate_pressed_event(static_cast<ForceSensor*>(robot.force_states[port->executeString(robot)])) == event) {
            return 1;
        }
        else return 0;
    }
};

class WhenDistance : public Block {
    Block* distance;
    Block* port;
    string option;
    string unit;
public:
    WhenDistance(Block* port, string option, Block* distance, string unit) : Block("Event", "WhenDistance"), port(port), option(option), distance(distance), unit(unit) {}

    double execute(Robot& robot) override {
        if (robot.distance_states.find(port->executeString(robot)) != robot.distance_states.end()
        && check_distance(robot.distance_states[port->executeString(robot)]->value == distance->execute(robot), option, distance->execute(robot))) {
            return 1;
        }
        else return 0;
    }
};

class WhenTilted : public Block {
    Block* angle;
public:
    WhenTilted(Block* angle) : Block("Event", "WhenTilted"), angle(angle) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement tilt detection
    }
};

class WhenOrientation : public Block {
    string orientation;
public:
    WhenOrientation(string orientation) : Block("Event", "WhenOrientation"), orientation(orientation) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement orientation detection
    }
};

class WhenGesture : public Block {
    string gesture;
public:
    WhenGesture(string gesture) : Block("Event", "WhenGesture"), gesture(gesture) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement gesture detection
    }
};

class WhenButton : public Block {
    string button;
    string event;
public:
    WhenButton(string button, string event) : Block("Event", "WhenButton"), button(button), event(event) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement button event detection
    }
};

class WhenTimer : public Block {
    Block* value;
public:
    WhenTimer(Block* value) : Block("Event", "WhenTimer"), value(value) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement timer event detection
    }
};

class WhenBroadcastReceived : public Block {
    string message;
public:
    WhenBroadcastReceived(string message) : Block("Event", "WhenBroadcastReceived"), message(message) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement broadcast event detection
    }
};

class Broadcast : public Block {
    Block* message;
public:
    Broadcast(Block* message) : Block("Event", "Broadcast"), message(message) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement broadcast sending
    }
};

class BroadcastAndWait : public Block {
    Block* message;
public:
    BroadcastAndWait(Block* message) : Block("Event", "BroadcastAndWait"), message(message) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement broadcast sending and waiting for response
    }
};
//--------------------------------------------

//-------------OPERATOR BLOCKS----------------
class PickRandom : public Block {
    Block* from;
    Block* to;
public:
    PickRandom(Block* from, Block* to) : Block("Operator", "PickRandom"), from(from), to(to) {}

    double execute(Robot& robot) override {
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

    double execute(Robot& robot) override {
        return value1->execute(robot) + value2->execute(robot);
    }
};

class Subtract : public Block {
    Block* value1;
    Block* value2;
public:
    Subtract(Block* value1, Block* value2) : Block("Operator", "Subtract"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) - value2->execute(robot);
    }
};

class Multiply : public Block {
    Block* value1;
    Block* value2;
public:
    Multiply(Block* value1, Block* value2) : Block("Operator", "Multiply"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) * value2->execute(robot);
    }
};

class Divide : public Block {
    Block* value1;
    Block* value2;
public:
    Divide(Block* value1, Block* value2) : Block("Operator", "Divide"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) / value2->execute(robot);
    }
};

//Some of these return a binary value, and some a number or string, depending on the context in which they are used
class LessThan : public Block {
    Block* value1;
    Block* value2;
public:
    LessThan(Block* value1, Block* value2) : Block("Operator", "LessThan"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) < value2->execute(robot);
    }

    string executeString(Robot& robot) override {
        if(value1->execute(robot) < value2->execute(robot)){
            return "True";
        } else {
            return "False";
        }
    }
};

class Equals : public Block {
    Block* value1;
    Block* value2;
public:
    Equals(Block* value1, Block* value2) : Block("Operator", "Equals"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) == value2->execute(robot);
    }

    string executeString(Robot& robot) override {
        if(value1->execute(robot) == value2->execute(robot)){
            return "True";
        } else {
            return "False";
        }
    }
};

class GreaterThan : public Block {
    Block* value1;
    Block* value2;
public:
    GreaterThan(Block* value1, Block* value2) : Block("Operator", "GreaterThan"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) > value2->execute(robot);
    }

    string executeString(Robot& robot) override {
        if(value1->execute(robot) > value2->execute(robot)){
            return "True";
        } else {
            return "False";
        }
    }
};

class And : public Block {
    Block* value1;
    Block* value2;
public:
    And(Block* value1, Block* value2) : Block("Operator", "And"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) && value2->execute(robot);
    }

    string executeString(Robot& robot) override {
        if(value1->execute(robot) && value2->execute(robot)){
            return "True";
        } else {
            return "False";
        }
    }
};

class Or : public Block {
    Block* value1;
    Block* value2;
public:
    Or(Block* value1, Block* value2) : Block("Operator", "Or"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) || value2->execute(robot);
    }

    string executeString(Robot& robot) override {
        if(value1->execute(robot) || value2->execute(robot)){
            return "True";
        } else {
            return "False";
        }
    }
};

class Not : public Block {
    Block* value;
public:
    Not(Block* value) : Block("Operator", "Not"), value(value) {}

    double execute(Robot& robot) override {
        return !value->execute(robot);
    }

    string executeString(Robot& robot) override {
        if(!value->execute(robot)){
            return "True";
        } else {
            return "False";
        }
    }
};

class IsInBetween : public Block {
    Block* value1;
    Block* value2;
    Block* value3;
public:
    IsInBetween(Block* value1, Block* value2, Block* value3) : Block("Operator", "IsInBetween"), value1(value1), value2(value2), value3(value3) {}

    double execute(Robot& robot) override {
        return value1->execute(robot) < value2->execute(robot) && value2->execute(robot) < value3->execute(robot);
    }

    string executeString(Robot& robot) override {
        if(value1->execute(robot) < value2->execute(robot) && value2->execute(robot) < value3->execute(robot)){
            return "True";
        } else {
            return "False";
        }
    }
};

class Join : public Block {
    Block* value1;
    Block* value2;
public:
    Join(Block* value1, Block* value2) : Block("Operator", "Join"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement string joining
    }

    string executeString(Robot& robot) override {
        return value1->executeString(robot) + value2->executeString(robot);
    }
};

class LetterOf : public Block {
    Block* value1;
    Block* value2;
public:
    LetterOf(Block* value1, Block* value2) : Block("Operator", "LetterOf"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement string letter extraction
    }

    string executeString(Robot& robot) override {
        return value1->executeString(robot).substr(value2->execute(robot), 1);
    }
};

class LengthOf : public Block {
    Block* value;
public:
    LengthOf(Block* value) : Block("Operator", "LengthOf"), value(value) {}

    double execute(Robot& robot) override {
        return value->executeString(robot).length();
    }
};

class Contains : public Block {
    Block* value1;
    Block* value2;
public:
    Contains(Block* value1, Block* value2) : Block("Operator", "Contains"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return value1->executeString(robot).find(value2->executeString(robot)) != string::npos;
    }

    string executeString(Robot& robot) override {
        if(value1->executeString(robot).find(value2->executeString(robot)) != string::npos){
            return "True";
        } else {
            return "False";
        }
    }
};

class Modulus : public Block {
    Block* value1;
    Block* value2;
public:
    Modulus(Block* value1, Block* value2) : Block("Operator", "Modulus"), value1(value1), value2(value2) {}

    double execute(Robot& robot) override {
        return static_cast<int>(value1->execute(robot)) % static_cast<int>(value2->execute(robot));
    }
};

class Round : public Block {
    Block* value;
public:
    Round(Block* value) : Block("Operator", "Round"), value(value) {}

    double execute(Robot& robot) override {
        return round(value->execute(robot));
    }
};

class MathOp : public Block {
    Block* value;
    string function_name;
public:
    MathOp(Block* value, string function_name) : Block("Operator", "MathOp"), value(value), function_name(function_name) {}

    double execute(Robot& robot) override {
        if(function_name == "abs"){
            return abs(value->execute(robot));
        } else if(function_name == "floor"){
            return floor(value->execute(robot));
        } else if (function_name == "ceiling"){
            return ceil(value->execute(robot));
        } else if (function_name == "sqrt"){
            return sqrt(value->execute(robot));
        } else if(function_name == "sin"){
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
        } else if(function_name == "e ^"){
            return exp(value->execute(robot));
        } else if(function_name == "10 ^"){
            return pow(10, value->execute(robot));
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

    double execute(Robot& robot) override {
        return value;
    }
};

class BlankBlockDouble : public Block {
    double value;
public:
    BlankBlockDouble(double value) : Block("Blank", "BlankBlockDouble"), value(value) {}

    double execute(Robot& robot) override {
        return value;
    }
};

class BlankBlockString : public Block {
    string value;
public:
    BlankBlockString(string value) : Block("Blank", "BlankBlockString"), value(value) {}

    double execute(Robot& robot) override {
        return 0;
    }

    string executeString(Robot& robot) override {
        return value;
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

    double execute(Robot& robot) override {
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[0]]->device_type != "Motor" 
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[1]]->device_type != "Motor" 
        || robot.movement_motors[0] == robot.movement_motors[1]
        || !is_number(value->executeString(robot))) {
            return 0;
        }

        if(forward){ // TODO: check if this is correct
            robot.motor_states[robot.movement_motors[0]] -> value = robot.movement_speed;
            robot.motor_states[robot.movement_motors[1]] -> value = -robot.movement_speed;
        } else {
            robot.motor_states[robot.movement_motors[0]] -> value = -robot.movement_speed;
            robot.motor_states[robot.movement_motors[1]] -> value = robot.movement_speed;
        }

        return convert_to_seconds(robot, unit, value->execute(robot));
    }

    void finish(Robot& robot) override {
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[0]]->device_type != "Motor" 
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[1]]->device_type != "Motor" 
        || robot.movement_motors[0] == robot.movement_motors[1]
        || !is_number(value->executeString(robot))) {
            return;
        }
        robot.motor_states[robot.movement_motors[0]] -> value = 0;
        robot.motor_states[robot.movement_motors[1]] -> value = 0;
        return;
    }
    
};

class StartMove : public Block {
    bool forward;

public:
    StartMove(bool forward) : Block("Move", "StartMove"), forward(forward) {}

    double execute(Robot& robot) override {
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[0]]->device_type != "Motor" 
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[1]]->device_type != "Motor" 
        || robot.movement_motors[0] == robot.movement_motors[1]) {
            return 0;
        }

        if(forward){ // TODO: check if this is correct
            robot.motor_states[robot.movement_motors[0]] -> value = robot.movement_speed;
            robot.motor_states[robot.movement_motors[1]] -> value = -robot.movement_speed;
        } else {
            robot.motor_states[robot.movement_motors[0]] -> value = -robot.movement_speed;
            robot.motor_states[robot.movement_motors[1]] -> value = robot.movement_speed;
        }

        return -1;
    }

    void finish(Robot& robot) override {
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[0]]->device_type != "Motor" 
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[1]]->device_type != "Motor" 
        || robot.movement_motors[0] == robot.movement_motors[1]) {
            return;
        }
        robot.motor_states[robot.movement_motors[0]] -> value = 0;
        robot.motor_states[robot.movement_motors[1]] -> value = 0;
        return;
    }
};

class Steer : public Block {
    Block* direction;
    Block* value;
    string unit;

public:
    Steer(Block* direction, Block* value, string unit) : Block("Move", "Steer"), direction(direction), value(value), unit(unit) {}

    double execute(Robot& robot) override { // TODO : nemma pojma sto radit s direction --> skuzi i popravi
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[0]]->device_type != "Motor" 
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[1]]->device_type != "Motor" 
        || robot.movement_motors[0] == robot.movement_motors[1]
        || !is_number(value->executeString(robot))) {
            return 0;
        }

        robot.motor_states[robot.movement_motors[0]] -> value = robot.movement_speed * min(1 - direction->execute(robot)/100, 1.0);
        robot.motor_states[robot.movement_motors[1]] -> value = robot.movement_speed * min(1 + direction->execute(robot)/100, 1.0);

        return convert_to_seconds(robot, unit, value->execute(robot));
    }

    void finish(Robot& robot) override {
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[0]]->device_type != "Motor" 
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[1]]->device_type != "Motor" 
        || robot.movement_motors[0] == robot.movement_motors[1]
        || !is_number(value->executeString(robot))) {
            return;
        }
        robot.motor_states[robot.movement_motors[0]] -> value = 0;
        robot.motor_states[robot.movement_motors[1]] -> value = 0;
        return;
    }
};

class StartSteer : public Block {
    Block* direction;

public:
    StartSteer(Block* direction) : Block("Move", "StartSteer"), direction(direction) {}

    double execute(Robot& robot) override { // TODO : tu isti direction je jedan veliki ?
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[0]]->device_type != "Motor" 
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[1]]->device_type != "Motor" 
        || robot.movement_motors[0] == robot.movement_motors[1]) {
            return 0;
        }

        robot.motor_states[robot.movement_motors[0]] -> value = robot.movement_speed * min(1 - direction->execute(robot)/100, 1.0);
        robot.motor_states[robot.movement_motors[1]] -> value = robot.movement_speed * min(1 + direction->execute(robot)/100, 1.0);

        return -1;
    }

    void finish(Robot& robot) override {
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[0]]->device_type != "Motor" 
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[1]]->device_type != "Motor" 
        || robot.movement_motors[0] == robot.movement_motors[1]) {
            return;
        }
        robot.motor_states[robot.movement_motors[0]] -> value = 0;
        robot.motor_states[robot.movement_motors[1]] -> value = 0;
        return;
    }
};

class StopMoving : public Block {
public:
    StopMoving() : Block("Move", "StopMoving") {}

    double execute(Robot& robot) override {
        if (robot.motor_states.find(robot.movement_motors[0]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[0]]->device_type != "Motor" 
        || robot.motor_states.find(robot.movement_motors[1]) == robot.motor_states.end()
        || robot.motor_states[robot.movement_motors[1]]->device_type != "Motor" 
        || robot.movement_motors[0] == robot.movement_motors[1]) {
            return 0;
        }

        robot.motor_states[robot.movement_motors[0]] -> value = 0;
        robot.motor_states[robot.movement_motors[1]] -> value = 0;

        return 0;
    }
};

class SetMovementSpeed : public Block {
    Block* speed;
public:
    SetMovementSpeed(Block* speed) : Block("Move", "SetMovementSpeed"), speed(speed) {}

    double execute(Robot& robot) override {
        robot.movement_speed = speed->execute(robot);
        return 0;
    }
};

class SetMovementPair : public Block { // TODO : change so this only marks these two ports as wheels, not the wheels as being on these two ports
    Block* pair;
public:
    SetMovementPair(Block* pair) : Block("Move", "SetMovementPair"), pair(pair) {}
    double execute(Robot& robot) override { //TODO
        if (pair->executeString(robot).length() == 2){
            robot.movement_motors[0] = pair->executeString(robot)[0];
            robot.movement_motors[1] = pair->executeString(robot)[1];
        } 
        else{
            robot.movement_motors[0] = "A";
            robot.movement_motors[1] = "B";
        }
        return 0;
    }
};

class SetMotorRotation : public Block { // TODO : pravo da kazem jako sam zbunjen oko toga sto tocno ovo radi
    string unit;
    Block* value;
public:
    SetMotorRotation(string unit, Block* value) : Block("Move", "SetMotorRotation"), unit(unit), value(value) {}

    double execute(Robot& robot) override { // TODO
        return 0;
    }
};
//--------------------------------------------

//-------------DISPLAY BLOCKS-----------------
class DisplayImageForTime : public Block {
    Block* image;
    double time;
public:
    DisplayImageForTime(Block* image, double time) : Block("Display", "DisplayImageForTime"), image(image), time(time) {}

    double execute(Robot& robot) override { // TODO : image mora biti Block*
        string str_image = image->executeString(robot);
        for(int i = 0; i < 25; ++i){
            if(i >= str_image.length()){
                str_image += "0";
            }
            if(isdigit(str_image[i])) str_image[i] = '0';
        }
        for(int i = 0; i < 5; ++i){
            for(int j = 0; j < 5; ++j){
                robot.pixel_display[i][j] = str_image[i*5 + j] * 10 * 10 / 9;
            }
        }
        return time;
    }

    void finish(Robot& robot) override {
        for(int i = 0; i < 5; ++i){
            for(int j = 0; j < 5; ++j){
                robot.pixel_display[i][j] = 0;
            }
        }
        return;
    }
};

class DisplayImage : public Block {
    Block* image;
public:
    DisplayImage(Block* image) : Block("Display", "DisplayImage"), image(image) {}

    double execute(Robot& robot) override {
        string str_image = image->executeString(robot);
        for(int i = 0; i < 25; ++i){
            if(i >= str_image.length()){
                str_image += "0";
            }
            if(isdigit(str_image[i])) str_image[i] = '0';
        }
        for(int i = 0; i < 5; ++i){
            for(int j = 0; j < 5; ++j){
                robot.pixel_display[i][j] = str_image[i*5 + j] * 10 * 10 / 9;
            }
        }
        return -1;
    }
};

class DisplayText : public Block { // TODO
    string text;
public:
    DisplayText(string text) : Block("Display", "DisplayText"), text(text) {}
    double execute(Robot& robot) override { // TODO
        return 0;
    }
};

class DisplayOff : public Block {
public:
    DisplayOff() : Block("Display", "DisplayOff") {}

    double execute(Robot& robot) override {
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

    double execute(Robot& robot) override { // TODO
        
        return 0;
    }
};

class SetPixel : public Block {
    Block* x;
    Block* y;
    Block* brightness;
public:
    SetPixel(Block* x, Block* y, Block* brightness) : Block("Display", "SetPixel"), x(x), y(y), brightness(brightness) {}

    double execute(Robot& robot) override {
        if(!is_number(x->executeString(robot)) || !is_number(y->executeString(robot))
        || x->execute(robot) < 1 || x->execute(robot) > 5
        || y->execute(robot) < 1 || y->execute(robot) > 5
        || !is_number(brightness->executeString(robot))
        || brightness->execute(robot) < 0 || brightness->execute(robot) > 100){
            return 0;
        }
        robot.pixel_display[static_cast<int>(x->execute(robot)-1)][static_cast<int>(y->execute(robot)-1)] = min(brightness->execute(robot), 100.0);
        return 0;
    }
};

class DisplayRotate : public Block { // TODO
    bool forward;
public:
    DisplayRotate(bool forward) : Block("Display", "DisplayRotate"), forward(forward) {}
    double execute(Robot& robot) override { // TODO
        if(forward) rotate_matrix_right(robot);
        else rotate_matrix_left(robot);
        return 0;
    }
};

class DisplaySetorientation : public Block { // TODO : provjeri jer nisam 100% convinced
    Block* orientation;
public:
    DisplaySetorientation(Block* orientation) : Block("Display", "DisplaySetorientation"), orientation(orientation) {}
    double execute(Robot& robot) override {
        if(!is_number(orientation->executeString(robot)) || orientation->execute(robot) < 1 || orientation->execute(robot) > 4){
            return 0;
        }
        while(robot.absolute_image_position != static_cast<int>(orientation->execute(robot)-1)){
            rotate_matrix_right(robot);
            robot.absolute_image_position = (robot.absolute_image_position + 1) % 4;
        }
        return 0;
    }
};

class CenterButtonLight : public Block {
    Block* color;

public:
    CenterButtonLight(Block* color) : Block("Display", "CenterButtonLight"), color(color) {}

    double execute(Robot& robot) override {
        if(!is_number(color->executeString(robot))) robot.button_color = 0;
        else robot.button_color = color->execute(robot);
        return 0;
    }
};

class UltrasonicSensorLight : public Block { // TODO : kad budem mogao isprobat
    string color;
    string port;

public:
    UltrasonicSensorLight(string color, string port) : Block("Display", "UltrasonicSensorLight"), color(color), port(port) {}

    double execute(Robot& robot) override {
        return 0;
    }
};
//--------------------------------------------


//-------------SOUND BLOCKS-------------------
class PlayUntilDone : public Block { // TODO : finish
    string sound_name;
    string sound_location;
public:
    PlayUntilDone(string sound, string sound_location) : Block("Sound", "PlayUntilDone"), sound_name(sound_name), sound_location(sound_location) {};
    double execute(Robot& robot) override { // TODO
        return 0;
    }
};

class Play : public Block { // TODO : finish
    string sound_name;
    string sound_location;
public:
    Play(string sound, string sound_location) : Block("Sound", "Play"), sound_name(sound_name), sound_location(sound_location) {};
    double execute(Robot& robot) override { // TODO
        return 0;
    }
};

class BeepForTime : public Block {
    Block* time;
    Block* frequency;
public:
    BeepForTime(Block* time, Block* frequency) : Block("Sound", "BeepForTime"), time(time), frequency(frequency) {}

    double execute(Robot& robot) override {
        if(!is_number(frequency->executeString(robot)) || frequency->execute(robot) < 48 || frequency->execute(robot) > 108){
            return 0;
        }
        if(!is_number(time->executeString(robot)) || time->execute(robot) <= 0){
            return 0;
        }
        robot.sound_state = "beep" + to_string(static_cast<int>(frequency->execute(robot)));
        return time->execute(robot);
    }

    void finish(Robot& robot) override {
        robot.sound_state = "";
        return;
    }
};

class Beep : public Block {
    Block* frequency;
public:
    Beep(Block* frequency) : Block("Sound", "Beep"), frequency(frequency) {}

    double execute(Robot& robot) override {
        if(!is_number(frequency->executeString(robot)) || frequency->execute(robot) < 48 || frequency->execute(robot) > 108){
            return 0;
        }
        robot.sound_state = "beep" + to_string(static_cast<int>(frequency->execute(robot)));
        return -1;
    }
};

class StopSound : public Block {
public:
    StopSound() : Block("Sound", "StopSound") {}

    double execute(Robot& robot) override {
        robot.sound_state = "";
        return 0;
    }
};

class ChangeEffectBy : public Block { // TODO : nemam pojma sto ovo tocno radi
    string effect;
    double value;
public:
    ChangeEffectBy(string effect, double value) : Block("Sound", "ChangeEffectBy"), effect(effect), value(value) {}

    double execute(Robot& robot) override { // TODO
        return 0;
    }
};

class SetEffectTo : public Block { // TODO : nemam pojma sto ovo tocno radi
    string effect;
    double value;
public:
    SetEffectTo(string effect, double value) : Block("Sound", "SetEffectTo"), effect(effect), value(value) {}

    double execute(Robot& robot) override { // TODO
        return 0;
    }
};

class ClearEffects : public Block { // TODO : nemam pojma sto ovo tocno radi
public:
    ClearEffects() : Block("Sound", "ClearEffects") {}

    double execute(Robot& robot) override {
        return 0;
    }
};

class ChangeVolumeBy : public Block { // TODO : provjeri
    Block* value;
public:
    ChangeVolumeBy(Block* value) : Block("Sound", "ChangeVolumeBy"), value(value) {}

    double execute(Robot& robot) override {
        if(!is_number(value->executeString(robot))){
            return 0;
        }
        robot.volume += value->execute(robot);
        return 0;
    }
};

class SetVolumeTo : public Block {// TODO : provjeri
    Block* value;
public:
    SetVolumeTo(Block* value) : Block("Sound", "SetVolumeTo"), value(value) {}

    double execute(Robot& robot) override {
        if(!is_number(value->executeString(robot))){
            return 0;
        }
        robot.volume = value->execute(robot);
        return 0;
    }
};

class Volume : public Block {
public:
    Volume() : Block("Sound", "Volume") {}

    double execute(Robot& robot) override {
        return robot.volume;
    }
};
//--------------------------------------------
//-----------MOTOR BLOCKS--------------------
class MotorTurnForDirection : public Block {
    Block* port;
    bool forward;
    Block* value;
    string unit;
public:
    MotorTurnForDirection(Block* port, Block* value, bool forward, string unit) : Block("Motor", "MotorTurnForDirection"), port(port), value(value), forward(forward), unit(unit) {}

    double execute(Robot& robot) override { 
        string good_ports = parse_port(robot, port->executeString(robot), "Motor");
        if(good_ports == "") return 0;
        if(!is_number(value->executeString(robot))) return 0;
        for(int i = 0; i < good_ports.length(); ++i){
            if(robot.motor_states.find(string(1, good_ports[i])) != robot.motor_states.end()){
                robot.motor_states[string(1, good_ports[i])]->value = robot.motor_states[string(1, good_ports[i])]->speed * (forward ? 1 : -1);
            }
        }
        return convert_to_seconds(robot, unit, value->execute(robot));
    }
};

class MotorGoDirectionToPosition : public Block {
    Block* port;
    string direction;
    Block* position;

    string good_ports;
    bool forward;

    bool first_time = true;
public:
    MotorGoDirectionToPosition(Block* port, string direction, Block* position) : Block("Motor", "MotorGoDirectionToPosition"), port(port), direction(direction), position(position) {}

    double execute(Robot& robot) override { 
        if(first_time){
            if(port == nullptr || port->executeString(robot) == ""){
                return 0;
            }
            helper(robot);
            first_time = false;
        }
        string good_ports2 = good_ports;
        if(good_ports == ""){
            return 0;
        }
        for(int i = 0; i < good_ports.length(); ++i){
            if(robot.motor_states.find(string(1, good_ports[i])) != robot.motor_states.end()){
                if(robot.motor_states[string(1, good_ports[i])]->position < 1.0 || robot.motor_states[string(1, good_ports[i])]->position > 359.0){
                    robot.motor_states[string(1, good_ports[i])]->value = 0;
                    good_ports2.erase(i, 1);
                }
                else robot.motor_states[string(1, good_ports[i])]->value = robot.motor_states[string(1, good_ports[i])]->speed * (forward ? 1 : -1);
            }
        }
        if(good_ports2 == ""){
            return 0;
        }
        good_ports = good_ports2;
        return robot.discrete_time_interval;
    }

    void helper(Robot& robot) {
        forward = calculate_direction(robot, port->executeString(robot), direction, position->execute(robot));
        good_ports = parse_port(robot, port->executeString(robot), "Motor");
    }

    bool done(){
        return good_ports == "";
    }
};

class MotorStartDirection : public Block {
    Block* port;
    bool forward;
public:
    MotorStartDirection(Block* port, bool forward) : Block("Motor", "MotorStartDirection"), port(port), forward(forward) {}

    double execute(Robot& robot) override { 
        string good_ports = parse_port(robot, port->executeString(robot), "Motor");
        if(good_ports == "") return 0;
        for(int i = 0; i < good_ports.length(); ++i){
            if(robot.motor_states.find(string(1, good_ports[i])) != robot.motor_states.end()){
                robot.motor_states[string(1, good_ports[i])]->value = robot.motor_states[string(1, good_ports[i])]->speed * (forward ? 1 : -1);
            }
        }
        return -1;
    }
};

class MotorStop : public Block {
    Block* port;
public:
    MotorStop(Block* port) : Block("Motor", "MotorStop"), port(port) {}
    double execute(Robot& robot) override {
        string good_ports = parse_port(robot, port->executeString(robot), "Motor");
        if(good_ports == "") return 0;
        for(int i = 0; i < good_ports.length(); ++i){
            if(robot.motor_states.find(string(1, good_ports[i])) != robot.motor_states.end()){
                robot.motor_states[string(1, good_ports[i])]->value = 0;
            }
        }
        return 0;
    }
};

class MotorSetSpeed : public Block {
    Block* port;
    Block* speed;
public:
    MotorSetSpeed(Block* port, Block* speed) : Block("Motor", "MotorSetSpeed"), port(port), speed(speed) {}

    double execute(Robot& robot) override { 
        string good_ports = parse_port(robot, port->executeString(robot), "Motor");
        if(good_ports == "") return 0;
        if(!is_number(speed->executeString(robot))) return 0;
        for(int i = 0; i < good_ports.length(); ++i){
            if(robot.motor_states.find(string(1, good_ports[i])) != robot.motor_states.end()){
                robot.motor_states[string(1, good_ports[i])]->speed = speed->execute(robot);
            }
        }
        return 0;
    }
};

class MotorPosition : public Block {
    Block* port;
public:
    MotorPosition(Block* port) : Block("Motor", "MotorPosition"), port(port) {}

    double execute(Robot& robot) override { //TODO : provjeri jel ovo i ovo ispod okej
        string good_ports = parse_port(robot, port->executeString(robot), "Motor");
        if(good_ports == "") return 0;
        for(int i = 0; i < good_ports.length(); ++i){
            if(robot.motor_states.find(string(1, good_ports[i])) != robot.motor_states.end()){
                return robot.motor_states[string(1, good_ports[i])]->position;
            }
        }
        return 0;
    }
};

class MotorSpeed : public Block {
    Block* port;
public:
    MotorSpeed(Block* port) : Block("Motor", "MotorSpeed"), port(port) {}

    double execute(Robot& robot) override { //TODO
        string good_ports = parse_port(robot, port->executeString(robot), "Motor");
        if(good_ports == "") return 0;
        for(int i = 0; i < good_ports.length(); ++i){
            if(robot.motor_states.find(string(1, good_ports[i])) != robot.motor_states.end()){
                return robot.motor_states[string(1, good_ports[i])]->speed;
            }
        }
        return 0;
    }
};
//--------------------------------------------
//-----------CONTROL BLOCKS-------------------
class Wait : public Block {
    Block* time;
public:
    Wait(Block* time) : Block("Control", "Wait"), time(time) {}

    double execute(Robot& robot) override {
        return time->execute(robot);
    }
};

class Repeat : public Block {
    Block* times; // TODO: check if this needs to be calculated only once or every time
    BlockSequence* block_sequence;
    int counter;
    bool when_done = false;
public:
    Repeat(Block* times, BlockSequence* block_sequence) : Block("Control", "Repeat"), times(times), block_sequence(block_sequence), counter(0) {}

    double execute(Robot& robot) override {
        if (block_sequence == nullptr) {
            return 0;
        }
        if (block_sequence->get_current_block()->next == nullptr) {
            counter++;
            block_sequence->reset();
        }
        if(counter == times->execute(robot)){
            when_done = true;
            return 0;
        }
        block_sequence->execute(robot);
        int t = block_sequence->get_time_left();
        block_sequence->set_time_left(0);
        return t;
    }

    bool done() override {
        return when_done;
    }
};

class Forever : public Block {
    BlockSequence* block_sequence;
public:
    Forever(BlockSequence* block_sequence) : Block("Control", "Forever"), block_sequence(block_sequence) {}

    double execute(Robot& robot) override {
        if (block_sequence == nullptr) {
            return 0;
        }
        if (block_sequence->get_current_block()->next == nullptr) block_sequence->reset();

        block_sequence->execute(robot);
        int t = block_sequence->get_time_left();
        block_sequence->set_time_left(0);
        return t;
    }

    bool done() override {
        return false;
    }
};

class If : public Block {
    BlockSequence* block_sequence;
    bool condition_checked, when_done = false;
    Block* condition;
public:
    If(BlockSequence* block_sequence, Block* condition) : Block("Control", "If"), block_sequence(block_sequence), condition(condition) {}

    double execute(Robot& robot) override {
        if((condition->execute(robot) && !condition_checked) || condition_checked){
            if(block_sequence->get_current_block()->next == nullptr){
                when_done = true;
                block_sequence->reset();
            }
            condition_checked = true;
            block_sequence->execute(robot);
            int t = block_sequence->get_time_left();
            block_sequence->set_time_left(0);
            return t;
        }
        else{
            when_done = true;
        }
        return 0;
    }

    bool done() override {
        return when_done;
    }
};

class IfElse : public Block {
    BlockSequence* block_sequence1;
    BlockSequence* block_sequence2;
    bool condition_checked, else_checked, when_done = false;
    Block* condition;
public:
    IfElse(BlockSequence* block_sequence1, BlockSequence* block_sequence2, Block* condition) : Block("Control", "IfElse"), block_sequence1(block_sequence1), block_sequence2(block_sequence2), condition(condition) {}

    double execute(Robot& robot) override {
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
            when_done = true;
            block_sequence1->reset();
            block_sequence2->reset();
        }
        return t;
    }

    bool done() override {
        return when_done;
    }
};

class WaitUntil : public Block {
    Block* condition;
    bool when_done = false;
public:
    WaitUntil(Block* condition) : Block("Control", "WaitUntil"), condition(condition) {}

    double execute(Robot& robot) override {
        if(condition->execute(robot) == 1){
            when_done = true;
            return 0;
        }
        return -1;
    }

    bool done() override {
        return when_done;
    }
};

class RepeatUntil : public Block {
    Block* condition;
    BlockSequence* block_sequence;
    bool when_done = false;
public:
    RepeatUntil(Block* condition, BlockSequence* block_sequence) : Block("Control", "RepeatUntil"), condition(condition), block_sequence(block_sequence) {}

    double execute(Robot& robot) override {
        if (block_sequence == nullptr) {
            return 0;
        }
        if (block_sequence->get_current_block()->next == nullptr) {
            block_sequence->reset();
        }
        if(block_sequence->get_current_block() == block_sequence->get_starting_block() && condition->execute(robot) == 1){
            when_done = true;
            return 0;
        }
        block_sequence->execute(robot);
        int t = block_sequence->get_time_left();
        block_sequence->set_time_left(0);
        return t;
    }

    bool done() override {
        return when_done;
    }
};

class StopOtherStacks : public Block {
public:
    StopOtherStacks() : Block("Control", "StopOtherStacks") {}

    double execute(Robot& robot) override {
        return 0;
    }
};

class ControlStop : public Block {
    string option;
public:
    ControlStop(string option) : Block("Control", "ControlStop"), option(option) {}

    double execute(Robot& robot) override {
        return 0;
    }
};

//--------------------------------------------

//-----------SENSOR BLOCKS--------------------
class IsColor : public Block {
    Block* port;
    Block* color;
public:
    IsColor(Block* port, Block* color) : Block("Sensor", "IsColor"), port(port), color(color) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement color detection
    }
    //TODO : make the executeString funciton for this and all other boolean-type sensor blocks
};

class SensorColor : public Block {
    Block* port;
public:
    SensorColor(Block* port) : Block("Sensor", "SensorColor"), port(port) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement color detection
    }
};

class IsReflectivity : public Block {
    Block* port;
    string comparator;
    Block* value;
public:
    IsReflectivity(Block* port, string comparator, Block* value) : Block("Sensor", "IsReflectivity"), port(port), comparator(comparator), value(value) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement reflectivity detection
    }
};

class SensorReflectivity : public Block {
    Block* port;
public:
    SensorReflectivity(Block* port) : Block("Sensor", "SensorReflectivity"), port(port) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement reflectivity detection
    }
};

class IsPressed : public Block {
    Block* port;
    string operation;
public:
    IsPressed(Block* port, string operation) : Block("Sensor", "IsPressed"), port(port), operation(operation) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement button press detection
    }
};

class SensorForce : public Block {
    Block* port;
    string unit;
public:
    SensorForce(Block* port, string unit) : Block("Sensor", "SensorForce"), port(port), unit(unit) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement force detection
    }
};

class IsDistance : public Block {
    Block* port;
    string comparator;
    Block* value;
    string unit;
public:
    IsDistance(Block* port, string comparator, Block* value, string unit) : Block("Sensor", "IsDistance"), port(port), comparator(comparator), value(value), unit(unit) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement distance detection
    }
};

class SensorDistance : public Block {
    Block* port;
    string unit;
public:
    SensorDistance(Block* port, string unit) : Block("Sensor", "SensorDistance"), port(port), unit(unit) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement distance detection
    }
};

class IsTilted : public Block {
    Block* angle;
public:
    IsTilted(Block* angle) : Block("Sensor", "IsTilted"), angle(angle) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement tilt detection
    }
};

class IsOrientation : public Block {
    string orientation;
public:
    IsOrientation(string orientation) : Block("Sensor", "IsOrientation"), orientation(orientation) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement orientation detection
    }
};

class IsMotion : public Block {
    string motion;
public: 
    IsMotion(string motion) : Block("Sensor", "IsMotion"), motion(motion) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement motion detection
    }
};

class OrientationAxis : public Block {
    string axis;
public:
    OrientationAxis(string axis) : Block("Sensor", "OrientationAxis"), axis(axis) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement orientation axis detection
    }
};

class ResetYawAngle : public Block {
public:
    ResetYawAngle() : Block("Sensor", "ResetYawAngle") {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement yaw angle reset
    }
};

class IsButtonPressed : public Block {
    string button;
    string event;
public:
    IsButtonPressed(string button, string event) : Block("Sensor", "IsButtonPressed"), button(button), event(event) {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement button press detection
    }
};

class Timer : public Block {
public:
    Timer() : Block("Sensor", "Timer") {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement timer detection
    }
}; 

class ResetTimer : public Block {
public:
    ResetTimer() : Block("Sensor", "ResetTimer") {}

    double execute(Robot& robot) override {
        return 0; // TODO: implement timer reset
    }
};
//-------------------------------------------
//-----------END OF BLOCKS--------------------



//-----------FUNCTION MAP---------------------

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;

FunctionMap createFunctionMap() {
    FunctionMap functionMap;

    // Event blocks
    functionMap["flipperevents_whenProgramStarts"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<WhenProgramStarts>();
    };

    functionMap ["flipperevents_whenColor"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string color_name = json_object[name]["inputs"]["OPTION"][1];
        Block* color = functionMap[json_object[color_name]["opcode"]](json_object, color_name).release();
        
        return make_unique<WhenColor>(port, color);
    };

    functionMap ["flipperevents_whenPressed"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string event = json_object[name]["fields"]["OPTION"][0];

        return make_unique<WhenPressed>(port, event);
    };

    functionMap ["flipperevents_whenDistance"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string option = json_object[name]["inputs"]["COMPARATOR"][0];
        
        Block* distance;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            distance = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            distance = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }

        string unit = json_object[name]["fields"]["UNIT"][0];
        
        return make_unique<WhenDistance>(port, option, distance, unit);
    };

    functionMap ["flipperevents_whenTilted"] = [&functionMap](const json& json_object, const string& name) {
        string angle_name = json_object[name]["inputs"]["VALUE"][1];
        Block* angle = functionMap[json_object[angle_name]["opcode"]](json_object, angle_name).release();
        
        return make_unique<WhenTilted>(angle);
    };

    functionMap ["flipperevents_whenOrientation"] = [&functionMap](const json& json_object, const string& name) {
        string orientation = json_object[name]["fields"]["VALUE"][0];
        
        return make_unique<WhenOrientation>(orientation);
    }; 

    functionMap ["flipperevents_whenGesture"] = [&functionMap](const json& json_object, const string& name) {
        string gesture = json_object[name]["fields"]["EVENT"][0];
        
        return make_unique<WhenGesture>(gesture);
    };

    functionMap ["flipperevents_whenButton"] = [&functionMap](const json& json_object, const string& name) {
        string button = json_object[name]["fields"]["BUTTON"][0];
        string event = json_object[name]["fields"]["EVENT"][0];
        
        return make_unique<WhenButton>(button, event);
    };

    functionMap ["flipperevents_whenTimer"] = [&functionMap](const json& json_object, const string& name) {
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        
        return make_unique<WhenTimer>(value);
    };

    functionMap["event_whenbroadcastreceived"] = [&functionMap](const json& json_object, const string& name) {
        string broadcast = json_object[name]["fields"]["BROADCAST_OPTION"][0];
        return make_unique<WhenBroadcastReceived>(broadcast);
    };

    functionMap ["event_broadcast"] = [&functionMap](const json& json_object, const string& name) {
        Block* broadcast;
        if(json_object[name]["inputs"]["BROADCAST_INPUT"][0] == 1){
            broadcast = new BlankBlockString(json_object[name]["inputs"]["BROADCAST_INPUT"][1][1].get<string>());
        } else {
            string from_name = json_object[name]["inputs"]["BROADCAST_INPUT"][1];
            broadcast = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        return make_unique<Broadcast>(broadcast);
    };

    functionMap ["event_broadcastandwait"] = [&functionMap](const json& json_object, const string& name) {
        Block* broadcast;
        if(json_object[name]["inputs"]["BROADCAST_INPUT"][0] == 1){
            broadcast = new BlankBlockString(json_object[name]["inputs"]["BROADCAST_INPUT"][1][1].get<string>());
        } else {
            string from_name = json_object[name]["inputs"]["BROADCAST_INPUT"][1];
            broadcast = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        return make_unique<BroadcastAndWait>(broadcast);
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
        
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        
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
        Block* image = functionMap[json_object[matrix_name]["opcode"]](json_object, matrix_name).release();
        double time = stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>());
        return make_unique<DisplayImageForTime>(image, time);
    };

    functionMap["flipperlight_lightDisplayImageOn"] = [&functionMap](const json& json_object, const string& name) {
        string matrix_name = json_object[name]["inputs"]["MATRIX"][1];
        Block* image = functionMap[json_object[matrix_name]["opcode"]](json_object, matrix_name).release();
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
        Block* x = functionMap[json_object[x_name]["opcode"]](json_object, x_name).release();
        Block* y = functionMap[json_object[y_name]["opcode"]](json_object, y_name).release();
        
        Block* brightness;
        if(json_object[name]["inputs"]["BRIGHTNESS"][0] == 1){
            brightness = new BlankBlockDouble(stod(json_object[name]["inputs"]["BRIGHTNESS"][1][1].get<string>()));
        } else {
            string distance_name = json_object[name]["inputs"]["BRIGHTNESS"][1];
            brightness = functionMap[json_object[distance_name]["opcode"]](json_object, distance_name).release();
        }

        return make_unique<SetPixel>(x, y, brightness);
    };

    functionMap["flipperlight_lightDisplayRotate"] = [&functionMap](const json& json_object, const string& name) {
        bool forward;
        int args = json_object[name]["inputs"]["DIRECTION"][1];
        if(args != 1){
            forward = true;
        }
        else{
            string direction_name = json_object[name]["inputs"]["DIRECTION"][1];
            string fwd = json_object[direction_name]["fields"]["field_flippermotor_custom-icon-direction"][0];
            
            if(fwd == "counterclockwise") {
                forward = true;
            } else {
                forward = false;
            }
        }
        return make_unique<DisplayRotate>(forward);
    };

    functionMap["flipperlight_lightDisplaySetOrientation"] = [&functionMap](const json& json_object, const string& name) {
        string orientation_name = json_object[name]["inputs"]["ORIENTATION"][1];
        Block* orientation = functionMap[json_object[orientation_name]["opcode"]](json_object, orientation_name).release();
        return make_unique<DisplaySetorientation>(orientation);
    };

    functionMap["flipperlight_centerButtonLight"] = [&functionMap](const json& json_object, const string& name) {
        string color_name = json_object[name]["inputs"]["COLOR"][1];
        Block* color = functionMap[json_object[color_name]["opcode"]](json_object, color_name).release();
        return make_unique<CenterButtonLight>(color);
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
        Block* time;
        if(json_object[name]["inputs"]["DURATION"][0] == 1){
            time = new BlankBlockDouble(stod(json_object[name]["inputs"]["DURATION"][1][1].get<string>()));
        } else {
            string time_name = json_object[name]["inputs"]["DURATION"][1];
            time = functionMap[json_object[time_name]["opcode"]](json_object, time_name).release();
        }
        string frequency_name = json_object[name]["inputs"]["NOTE"][1];
        Block* frequency = functionMap[json_object[frequency_name]["opcode"]](json_object, frequency_name).release();
        return make_unique<BeepForTime>(time, frequency);
    };

    functionMap["flippersound_beep"] = [&functionMap](const json& json_object, const string& name) {
        string frequency_name = json_object[name]["inputs"]["NOTE"][1];
        Block* frequency = functionMap[json_object[frequency_name]["opcode"]](json_object, frequency_name).release();
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
        Block* volume;
        if(json_object[name]["inputs"]["VOLUME"][0] == 1){
            volume = new BlankBlockDouble(stod(json_object[name]["inputs"]["VOLUME"][1][1].get<string>()));
        } else {
            string volume_name = json_object[name]["inputs"]["VOLUME"][1];
            volume = functionMap[json_object[volume_name]["opcode"]](json_object, volume_name).release();
        }
        return make_unique<ChangeVolumeBy>(volume);
    };

    functionMap["sound_setvolumeto"] = [&functionMap](const json& json_object, const string& name) {
        Block* volume;
        if(json_object[name]["inputs"]["VOLUME"][0] == 1){
            volume = new BlankBlockDouble(stod(json_object[name]["inputs"]["VOLUME"][1][1].get<string>()));
        } else {
            string volume_name = json_object[name]["inputs"]["VOLUME"][1];
            volume = functionMap[json_object[volume_name]["opcode"]](json_object, volume_name).release();
        }
        return make_unique<SetVolumeTo>(volume);
    };

    functionMap ["sound_volume"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<Volume>();
    };
    //--------------------------------------------

    //Motor blocks
    functionMap["flippermotor_motorTurnForDirection"] = [&functionMap](const json& json_object, const string& name) {
        bool forward;
        int args = json_object[name]["inputs"]["DIRECTION"][1];
        if(args != 1){
            forward = true;
        }
        else{
            string direction_name = json_object[name]["inputs"]["DIRECTION"][1];
            string fwd = json_object[direction_name]["fields"]["field_flippermotor_custom-icon-direction"][0];
            
            if(fwd == "counterclockwise") {
                forward = true;
            } else {
                forward = false;
            }
        }
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();
        
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string from_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[from_name]["opcode"]](json_object, from_name).release();
        }
        return make_unique<MotorTurnForDirection>(port, value, forward, unit);
    };

    functionMap["flippermotor_motorGoDirectionToPosition"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        string direction = json_object[name]["fields"]["DIRECTION"][0].get<string>();
        
        string position_name = json_object[name]["inputs"]["POSITION"][1];
        Block* position = functionMap[json_object[position_name]["opcode"]](json_object, position_name).release();
        return make_unique<MotorGoDirectionToPosition>(port, direction, position);
    };

    functionMap ["flippermotor_motorStartDirection"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        bool forward;
        int args = json_object[name]["inputs"]["DIRECTION"][1];
        if(args != 1){
            forward = true;
        }
        else{
            string direction_name = json_object[name]["inputs"]["DIRECTION"][1];
            string fwd = json_object[direction_name]["fields"]["field_flippermotor_custom-icon-direction"][0];
            
            if(fwd == "counterclockwise") {
                forward = true;
            } else {
                forward = false;
            }
        }
        
        return make_unique<MotorStartDirection>(port, forward);
    };

    functionMap ["flippermotor_motorStop"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        return make_unique<MotorStop>(port);
    };

    functionMap ["flippermotor_motorSetSpeed"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        Block* speed;
        if(json_object[name]["inputs"]["SPEED"][0] == 1){
            speed = new BlankBlockDouble(stod(json_object[name]["inputs"]["SPEED"][1][1].get<string>()));
        } else {
            string speed_name = json_object[name]["inputs"]["SPEED"][1];
            speed = functionMap[json_object[speed_name]["opcode"]](json_object, speed_name).release();
        }
        return make_unique<MotorSetSpeed>(port, speed);
    };

    functionMap ["flippermotor_absolutePosition"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        return make_unique<MotorPosition>(port);
    };

    functionMap ["flippermotor_speed"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        return make_unique<MotorSpeed>(port);
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

        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            string key = json_object[name]["inputs"]["SUBSTACK"][1];
            BlockSequence* block_sequence = processBlock(json_object, key);
        }

        return make_unique<Repeat>(times, block_sequence);
    };

    functionMap["control_forever"] = [&functionMap](const json& json_object, const string& name) {
        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            string key = json_object[name]["inputs"]["SUBSTACK"][1];
            BlockSequence* block_sequence = processBlock(json_object, key);
        }

        return make_unique<Forever>(block_sequence);
    };

    functionMap ["control_if"] = [&functionMap](const json& json_object, const string& name) {
        Block* condition = nullptr;
        if (json_object[name]["inputs"].contains("CONDITION")) {
            string condition_name = json_object[name]["inputs"]["CONDITION"][1];
            condition = functionMap[json_object[condition_name]["opcode"]](json_object, condition_name).release();
        }

        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            string key = json_object[name]["inputs"]["SUBSTACK"][1];
            BlockSequence* block_sequence = processBlock(json_object, key);
        }

        return make_unique<If>(block_sequence, condition);
    };

    functionMap["control_if_else"] = [&functionMap](const json& json_object, const string& name) {
        Block* condition = nullptr;
        if (json_object[name]["inputs"].contains("CONDITION")) {
            string condition_name = json_object[name]["inputs"]["CONDITION"][1];
            condition = functionMap[json_object[condition_name]["opcode"]](json_object, condition_name).release();
        }

        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            string key = json_object[name]["inputs"]["SUBSTACK"][1];
            BlockSequence* block_sequence = processBlock(json_object, key);
        }

        BlockSequence* else_block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK2")) {
            string key = json_object[name]["inputs"]["SUBSTACK2"][1];
            BlockSequence* else_block_sequence = processBlock(json_object, key);
        }

        return make_unique<IfElse>(block_sequence, else_block_sequence, condition);
    };

    functionMap ["control_wait_until"] = [&functionMap](const json& json_object, const string& name) {
        Block* condition = nullptr;
        if (json_object[name]["inputs"].contains("CONDITION")) {
            string condition_name = json_object[name]["inputs"]["CONDITION"][1];
            condition = functionMap[json_object[condition_name]["opcode"]](json_object, condition_name).release();
        }

        return make_unique<WaitUntil>(condition);
    };

    functionMap ["control_repeat_until"] = [&functionMap](const json& json_object, const string& name) {
        Block* condition = nullptr;
        if (json_object[name]["inputs"].contains("CONDITION")) {
            string condition_name = json_object[name]["inputs"]["CONDITION"][1];
            condition = functionMap[json_object[condition_name]["opcode"]](json_object, condition_name).release();
        }

        BlockSequence* block_sequence = nullptr;
        if (json_object[name]["inputs"].contains("SUBSTACK")) {
            string key = json_object[name]["inputs"]["SUBSTACK"][1];
            BlockSequence* block_sequence = processBlock(json_object, key);
        }

        return make_unique<RepeatUntil>(condition, block_sequence);
    };

    functionMap ["flippercontrol_stopOtherStacks"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<StopOtherStacks>();
    };

    functionMap ["flippercontrol_stop"] = [&functionMap](const json& json_object, const string& name) {
        string option = json_object[name]["fields"]["STOP_OPTION"][0];
        return make_unique<ControlStop>(option);
    };
    //--------------------------------------------
    // Sensor blocks
    functionMap ["flippersensors_isColor"] = [&functionMap](const json& json_object, const string& name) {
        string color_name = json_object[name]["inputs"]["VALUE"][1];
        Block* color = functionMap[json_object[color_name]["opcode"]](json_object, color_name).release();
        
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        return make_unique<IsColor>(port, color);
    };

    functionMap ["flippersensors_color"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        return make_unique<SensorColor>(port);
    };

    functionMap ["flippersensors_isReflectivity"] = [&functionMap](const json& json_object, const string& name) {
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value1_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[value1_name]["opcode"]](json_object, value1_name).release();
        }

        string comparator = json_object[name]["fields"]["COMPARATOR"][0].get<string>();

        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        return make_unique<IsReflectivity>(port, comparator, value);
    };

    functionMap ["flippersensors_reflectivity"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        return make_unique<SensorReflectivity>(port);
    };

    functionMap ["flippersensors_isPressed"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string pressed = json_object[name]["fields"]["PRESSED"][0].get<string>();

        return make_unique<IsPressed>(port, pressed);
    }; 

    functionMap ["flippersensors_force"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();

        return make_unique<SensorForce>(port, unit);
    };

    functionMap ["flippersensors_isDistance"] = [&functionMap](const json& json_object, const string& name) {
        Block* value;
        if(json_object[name]["inputs"]["VALUE"][0] == 1){
            value = new BlankBlockDouble(stod(json_object[name]["inputs"]["VALUE"][1][1].get<string>()));
        } else {
            string value_name = json_object[name]["inputs"]["VALUE"][1];
            value = functionMap[json_object[value_name]["opcode"]](json_object, value_name).release();
        }

        string comparator = json_object[name]["fields"]["COMPARATOR"][0].get<string>();

        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();

        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();

        return make_unique<IsDistance>(port, comparator, value, unit);
    };

    functionMap ["flippersensors_distance"] = [&functionMap](const json& json_object, const string& name) {
        string port_name = json_object[name]["inputs"]["PORT"][1];
        Block* port = functionMap[json_object[port_name]["opcode"]](json_object, port_name).release();
        
        string unit = json_object[name]["fields"]["UNIT"][0].get<string>();

        return make_unique<SensorDistance>(port, unit);
    };

    functionMap ["flippersensors_isTilted"] = [&functionMap](const json& json_object, const string& name) {
        string angle_name = json_object[name]["inputs"]["VALUE"][1];
        Block* angle = functionMap[json_object[angle_name]["opcode"]](json_object, angle_name).release();
        
        return make_unique<IsTilted>(angle);
    };

    functionMap ["flippersensors_isorientation"] = [&functionMap](const json& json_object, const string& name) {
        string orientation = json_object[name]["fields"]["ORIENTATION"][0].get<string>();
        return make_unique<IsOrientation>(orientation);
    };

    functionMap ["flippersensors_ismotion"] = [&functionMap](const json& json_object, const string& name) {
        string motion = json_object[name]["fields"]["MOTION"][0].get<string>();
        return make_unique<IsMotion>(motion);
    };

    functionMap ["flippersensors_orientationAxis"] = [&functionMap](const json& json_object, const string& name) {
        string axis = json_object[name]["fields"]["AXIS"][0].get<string>();
        return make_unique<OrientationAxis>(axis);
    };

    functionMap ["flippersensors_resetYaw"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<ResetYawAngle>();
    };

    functionMap ["flippersensors_buttonIsPressed"] = [&functionMap](const json& json_object, const string& name) {
        string button = json_object[name]["fields"]["BUTTON"][0].get<string>();
        string event = json_object[name]["fields"]["EVENT"][0].get<string>();
        return make_unique<IsButtonPressed>(button, event);
    };

    functionMap ["flippersensors_timer"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<Timer>();
    };

    functionMap ["flippersensors_resetTimer"] = [&functionMap](const json& json_object, const string& name) {
        return make_unique<ResetTimer>();
    };

    //---------------------------------------------
    // Miscelanious helper "blocks"
    functionMap["flippermove_movement-port-selector"] = [&functionMap](const json& json_object, const string& name) {
        string port = json_object[name]["fields"]["field_flippermove_multiple-port-selector"][0].get<string>();
        return make_unique<BlankBlockString>(port);
    };

    functionMap ["flippermotor_multiple-port-selector"] = [&functionMap](const json& json_object, const string& name) {
        string port = json_object[name]["fields"]["field_flippermotor_multiple-port-selector"][0].get<string>();
        return make_unique<BlankBlockString>(port);
    };

    functionMap["flippermotor_custom-angle"] = [&functionMap](const json& json_object, const string& name) {
        string angle = json_object[name]["fields"]["field_flippermotor_custom-angle"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(angle));
    };

    functionMap ["flipperevents_color-selector"] = [&functionMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flipperevents_color-selector"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(color));
    };

    functionMap ["flipperevents_color-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flipperevents_color-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(color);
    };

    functionMap ["flipperevents_force-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string force = json_object[name]["fields"]["field_flipperevents_force-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(force);
    };

    functionMap ["flipperevents_distance-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string distance = json_object[name]["fields"]["field_flipperevents_distance-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(distance);
    };

    functionMap ["flipperevents_custom-tilted"] = [&functionMap](const json& json_object, const string& name) {
        string tilt = json_object[name]["fields"]["field_flipperevents_custom-tilted"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(tilt));
    };

    functionMap ["flippersensors_color-selector"] = [&functionMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flippersensors_color-selector"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(color));
    };

    functionMap ["flippersensors_color-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flippersensors_color-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(color);
    };

    functionMap ["flippersensors_force-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string force = json_object[name]["fields"]["field_flippersensors_force-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(force);
    };

    functionMap ["flippersensors_distance-sensor-selector"] = [&functionMap](const json& json_object, const string& name) {
        string distance = json_object[name]["fields"]["field_flippersensors_distance-sensor-selector"][0].get<string>();
        return make_unique<BlankBlockString>(distance);
    };

    functionMap ["flippersensors_custom-tilted"] = [&functionMap](const json& json_object, const string& name) {
        string tilt = json_object[name]["fields"]["field_flippersensors_custom-tilted"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(tilt));
    };

    functionMap["flipperlight_matrix-5x5-brightness-image"] = [&functionMap](const json& json_object, const string& name) {
        string image = json_object[name]["fields"]["field_flipperlight_matrix-5x5-brightness-image"][0].get<string>();
        return make_unique<BlankBlockString>(image);
    };

    functionMap["flipperlight_menu_orientation"] = [&functionMap](const json& json_object, const string& name) {
        string orientation = json_object[name]["fields"]["orientation"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(orientation));
    };

    functionMap["flipperlight_color-selector-vertical"] = [&functionMap](const json& json_object, const string& name) {
        string color = json_object[name]["fields"]["field_flipperlight_color-selector-vertical"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(color));
    };

    functionMap["flippersound_custom-piano"] = [&functionMap](const json& json_object, const string& name) {
        string note = json_object[name]["fields"]["field_flippersound_custom-piano"][0].get<string>();
        return make_unique<BlankBlockInt>(stoi(note));
    };

    //TODO : add the rest of the blocks when i know what to do w conditions

    return functionMap;
}

FunctionMap functionMap = createFunctionMap();


inline BlockSequence* processBlock(const json& blocks, string key) {
    auto curr_block = blocks[key];
    if (!curr_block.contains("opcode")) {
        cerr << "Error: Block is missing 'opcode'!" << endl;
        return nullptr;
    }
    auto curr_sequence_block = functionMap[curr_block["opcode"]](blocks, key).release();
    BlockSequence* block_sequence = new BlockSequence(curr_sequence_block);

    while (true) {
        if (!blocks.contains(curr_block["next"]) || curr_block["next"].is_null()) {
            break;
        }
        auto next_block = blocks[curr_block["next"]];
        if (!next_block.contains("opcode")) {
            cerr << "Error: Block is missing 'opcode'!" << endl;
            return nullptr;
        }
        auto next_sequence_block = functionMap[next_block["opcode"]](blocks, curr_block["next"]).release();
        curr_sequence_block->next = next_sequence_block;
        next_sequence_block->parent = curr_sequence_block;
        curr_sequence_block = next_sequence_block;
        curr_block = next_block;
    }

    return block_sequence;
}


#endif // BLOCKS_H