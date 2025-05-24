#ifndef FUNCTIONMAP_HPP
#define FUNCTIONMAP_HPP
#include <map>
#include <functional>
#include <memory>
#include <string>

#include "control_blocks_parsing.hpp"
#include "event_blocks_parsing.hpp"
#include "miscelanious_blocks_parsing.hpp"
#include "operator_blocks_parsing.hpp"
#include "sensor_blocks_parsing.hpp"
#include "display_blocks_parsing.hpp"
#include "movement_blocks_parsing.hpp"
#include "motor_blocks_parsing.hpp"
#include "sound_blocks_parsing.hpp"

using namespace std;

using FunctionMap = map<string, function<unique_ptr<Block>(json, string)>>;
FunctionMap createFunctionMap() {
    FunctionMap functionMap;

    // Control blocks
    functionMap = createControlFunctionMap(functionMap);

    // Miscelanious blocks
    auto miscelaniousFunctionMap = createMiscelaniousFunctionMap(functionMap);
    functionMap.insert(miscelaniousFunctionMap.begin(), miscelaniousFunctionMap.end());

    // Event blocks
    auto eventFunctionMap = createEventFunctionMap(functionMap);
    functionMap.insert(eventFunctionMap.begin(), eventFunctionMap.end());

    // Operator blocks
    auto operatorFunctionMap = createOperatorFunctionMap(functionMap);
    functionMap.insert(operatorFunctionMap.begin(), operatorFunctionMap.end());

    // Sensor blocks
    auto sensorFunctionMap = createSensorFunctionMap(functionMap);
    functionMap.insert(sensorFunctionMap.begin(), sensorFunctionMap.end());

    // Display blocks
    auto displayFunctionMap = createDisplayFunctionMap(functionMap);
    functionMap.insert(displayFunctionMap.begin(), displayFunctionMap.end());

    // Movement blocks
    auto movementFunctionMap = createMovementFunctionMap(functionMap);
    functionMap.insert(movementFunctionMap.begin(), movementFunctionMap.end());

    // Motor blocks
    auto motorFunctionMap = createMotorFunctionMap(functionMap);
    functionMap.insert(motorFunctionMap.begin(), motorFunctionMap.end());

    // Sound blocks
    auto soundFunctionMap = createSoundFunctionMap(functionMap);
    functionMap.insert(soundFunctionMap.begin(), soundFunctionMap.end());

    return functionMap;
}

#endif

