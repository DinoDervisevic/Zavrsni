#ifndef JSON_EXPORT_HPP
#define JSON_EXPORT_HPP

#include <fstream>
#include <vector>
#include "robot/Robot.hpp"
#include "utils/obstacle.hpp"
#include "external/json.hpp"

using json = nlohmann::json;
using namespace std;

void export_simulation_to_json(
    const vector<RobotState>& robot_states,
    const vector<Obstacle>& obstacles,
    const vector<Wall>& walls,
    const string& output_path
) {
    json output;
    
    // Robot stanja
    json states_array = json::array();
    for (const auto& state : robot_states) {
        states_array.push_back({
            {"t", state.t},
            {"x", state.x},
            {"y", state.y},
            {"angle", state.angle}
        });
    }
    output["robot_states"] = states_array;
    
    // Obstacles
    json obstacles_array = json::array();
    for (const auto& obs : obstacles) {
        obstacles_array.push_back({
            {"x", obs.x},
            {"y", obs.y},
            {"width", obs.width},
            {"length", obs.length},
            {"angle", obs.angle}
        });
    }
    output["obstacles"] = obstacles_array;
    
    // Walls
    json walls_array = json::array();
    for (const auto& wall : walls) {
        walls_array.push_back({
            {"x1", wall.x1},
            {"y1", wall.y1},
            {"x2", wall.x2},
            {"y2", wall.y2}
        });
    }
    output["walls"] = walls_array;
    
    // Spremi u datoteku
    ofstream file(output_path);
    if (!file.is_open()) {
        cerr << "Greška: Ne mogu otvoriti datoteku za pisanje: " << output_path << endl;
        return;
    }
    file << output.dump(2);
    file.close();
    
    cout << "✓ Simulacija exportana u: " << output_path << endl;
}

#endif
