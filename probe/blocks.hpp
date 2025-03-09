#ifndef BLOCKS_H
#define BLOCKS_H

#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <algorithm>
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
};


//-------------MOVEMENT BLOCKS----------------
class Move : public Block {
    bool forward;
    double value;
    string unit;
public:
    Move(bool forward, double value, string unit) : Block("Move", "Move"), forward(forward), value(value), unit(unit) {}

    int execute(Robot& robot) override {
        if(forward){
            robot.v1 = robot.movement_speed;
            robot.v2 = robot.movement_speed;
        } else {
            robot.v1 = -robot.movement_speed;
            robot.v2 = -robot.movement_speed;
        }
        return convert_to_seconds(robot, unit, value);
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
    int direction;
    double value;
    string unit;

public:
    Steer(int direction, double value, string unit) : Block("Move", "Steer"), direction(direction), value(value), unit(unit) {}

    int execute(Robot& robot) override { // TODO : provjeri jel ovo dobro
        robot.v1 = robot.movement_speed * min(1 - direction/100, 1);
        robot.v2 = robot.movement_speed * min(1 + direction/100, 1);

        return convert_to_seconds(robot, unit, value);
    }
};

class StartSteer : public Block {
    int direction;

public:
    StartSteer(int direction) : Block("Move", "StartSteer"), direction(direction) {}

    int execute(Robot& robot) override {
        robot.v1 = robot.movement_speed * min(1 - direction/100, 1);
        robot.v2 = robot.movement_speed * min(1 + direction/100, 1);

        return -1;
    }
};

class StopMoving : public Block {
public:
    int execute(Robot& robot) override {
        robot.v1 = 0.0;
        robot.v2 = 0.0;
        return 0;
    }
};

class SetMovementSpeed : public Block {
    double speed;
public:
    SetMovementSpeed(double speed) : Block("Move", "SetMovementSpeed"), speed(speed) {}

    int execute(Robot& robot) override {
        robot.movement_speed = speed;
        return 0;
    }
};

class SetMovementPair : public Block { // TODO
    string left, right;
};

class SetMotorRotation : public Block { // TODO
    string unit;
    double value;
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
};

class DisplayOff : public Block {
public:
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
};

class SetPixel : public Block {
    int x, y;
    double brightness;
public:
    SetPixel(int x, int y, string color) : Block("Display", "SetPixel"), x(x), y(y), brightness(brightness) {}

    int execute(Robot& robot) override {
        robot.pixel_display[x][y] = brightness;
        return -1;
    }
};

class DisplayRotate : public Block { // TODO
    int direction;
};

class DisplaySetorientation : public Block { // TODO
    string orientation;
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
class PlayUntilDone : public Block { // TODO : figure out how to know how long the sound is
    string sound;
public:
    PlayUntilDone(string sound) : Block("Sound", "PlayUntilDone"), sound(sound) {}

    int execute(Robot& robot) override {
        robot.sound_state = sound;
        return rand() % 10 + 1;
    }
};

class Play : public Block {
    string sound;
public:
    Play(string sound) : Block("Sound", "Play"), sound(sound) {}

    int execute(Robot& robot) override {
        robot.sound_state = sound;
        return -1;
    }
};

class BeepForTime : public Block {
    double time;
    double frequency;
public:
    BeepForTime(double time) : Block("Sound", "BeepForTime"), time(time) {}

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
    int execute(Robot& robot) override {
        robot.sound_state = "";
        return 0;
    }
};

class ChangeEffectBy : public Block { // TODO
    string effect;
    double value;
};

class SetEffectTo : public Block { // TODO
    string effect;
    double value;
};

class ClearEffects : public Block { // TODO
public:
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
class MotorTurnForDirection : public Block {
public:
    double speed;
    MotorTurnForDirection(double speed) : Block("Motor", "MotorTurnForDirection"), speed(speed) {}

    int execute(Robot& robot) override {
        robot.v1 = speed;
        robot.v2 = speed;
        std::cout << "Robot se krece naprijed brzinom: " << speed << std::endl;
    }
};

class MotorStop : public Block {
public:
int execute(Robot& robot) override {
        robot.v1 = 0.0;
        robot.v2 = 0.0;
        return 0;
    }
};

using FunctionMap = std::map<std::string, std::function<std::unique_ptr<Block>(json)>>;

FunctionMap createFunctionMap() {
    FunctionMap functionMap;
    functionMap["flippermotor_motorTurnForDirection"] = [](json inputs) {
        double speed = inputs["VALUE"][1][1].get<double>();
        return std::make_unique<MotorTurnForDirection>(speed);
    };
    functionMap["flippermotor_motorStop"] = [](json inputs) {
        return std::make_unique<MotorStop>();
    };
    // Dodajte ostale funkcije prema potrebi
    return functionMap;
}

#endif // BLOCKS_H