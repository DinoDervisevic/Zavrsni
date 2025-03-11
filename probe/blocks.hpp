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
class Move : public Block { // TODO :  on this and every other move block, change speed of motors, not wheels (when you get the robot that is)
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

class SetMovementPair : public Block { // TODO : change so this only marks these two ports as wheels, not the wheels as being on these two ports
    string left, right;
public:
    SetMovementPair(string left, string right) : Block("Move", "SetMovementPair"), left(left), right(right) {}
    int execute(Robot& robot) override {
        robot.left_wheel = left;
        robot.right_wheel = right;
        return 0;
    }
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
        cout << "Robot se krece naprijed brzinom: " << speed << endl;
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

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;

FunctionMap createFunctionMap() {
    FunctionMap functionMap;

    // Movement blocks
    functionMap["move_move"] = [](json json, string name) {
        string direction_name = json[name]["inputs"]["DIRECTION"][1];
        string fwd = json[direction_name]["fields"]["field_flippermove_custom-icon-direction"][0];
        bool forward;
        if(fwd == "forward") {
            forward = true;
        } else {
            forward = false;
        }
        double value = stod(json[name]["inputs"]["VALUE"][1][1].get<string>());
        string unit = json[name]["fields"]["UNIT"][0];
        return make_unique<Move>(forward, value, unit);
    };

    functionMap["move_startmove"] = [](json json, string name) {
        string direction_name = json[name]["inputs"]["DIRECTION"][1];
        string fwd = json[direction_name]["fields"]["field_flippermove_custom-icon-direction"][0];
        bool forward;
        if(fwd == "forward") {
            forward = true;
        } else {
            forward = false;
        }
        return make_unique<StartMove>(forward);
    };

    functionMap["move_steer"] = [](json json, string name) {
        string direction_name = json[name]["inputs"]["STEERING"][1];
        int direction = stoi(json[direction_name]["fields"]["field_flippermove_rotation-wheel"][0].get<string>());
        double value = stod(json[name]["inputs"]["VALUE"][1][1].get<string>());
        string unit = json[name]["fields"]["UNIT"][0];
        return make_unique<Steer>(direction, value, unit);
    };

    functionMap["move_startsteer"] = [](json json, string name) {
        string direction_name = json[name]["inputs"]["STEERING"][1];
        int direction = stoi(json[direction_name]["fields"]["field_flippermove_rotation-wheel"][0].get<string>());
        return make_unique<StartSteer>(direction);
    };

    functionMap["move_stopmoving"] = [](json json, string name) {
        return make_unique<StopMoving>();
    };

    functionMap["move_setmovementspeed"] = [](json json, string name) {
        double speed = stod(json[name]["fields"]["SPEED"][1][1].get<string>());
        return make_unique<SetMovementSpeed>(speed);
    };

    functionMap["move_setmovementpair"] = [](json json, string name) {
        string pair_name = json[name]["inputs"]["PAIR"][1];
        string pair = json[pair_name]["fields"]["field_flippermove_movement-port-selector"][0];
        return make_unique<SetMovementPair>(string(1, pair[0]), string(1, pair[1]));
    };

    functionMap["move_setmotorrotation"] = [](json json, string name) {
        string unit = json[name]["fields"]["UNIT"][0];
        string distance_name = json[name]["inputs"]["DISTANCE"][1];
        double value = stod(json[distance_name]["fields"]["field_flippermove_custom-set-move-distance-number"][0].get<string>());
        return make_unique<SetMotorRotation>(unit, value);
    };
    //--------------------------------------------
    // Display blocks
    functionMap["display_displayimagefortime"] = [](json json, string name) {
        string matrix_name = json[name]["inputs"]["MATRIX"][1];
        string image = json[matrix_name]["fields"]["field_flipperlight_matrix-5x5-brightness-image"][0];
        double time = stod(json[name]["inputs"]["VALUE"][1][1].get<string>());
        return make_unique<DisplayImageForTime>(image, time);
    };

    functionMap["display_displayimage"] = [](json json, string name) {
        string matrix_name = json[name]["inputs"]["MATRIX"][1];
        string image = json[matrix_name]["fields"]["field_flipperlight_matrix-5x5-brightness-image"][0];
        return make_unique<DisplayImage>(image);
    };

    return functionMap;
}

#endif // BLOCKS_H