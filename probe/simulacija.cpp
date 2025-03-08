#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <fstream>
#include <algorithm>
#include <set>
#include <queue>
#include <sstream>
#include "json.hpp"
#include "robot.hpp"

using namespace std;

using json = nlohmann::json;

double time = 0; // current time in seconds



void start_simulation(Robot& robot, json blocks) {
}


int main() {
    string json_file_path = "C:/Users/amrad/OneDrive/Documents/LEGO Education SPIKE/project.json";

    ifstream file(json_file_path);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return -1;
    }

    json j;
    file >> j;
    auto blocks = j["targets"][1]["blocks"];

    Robot robot("robot", 0, 0);

    start_simulation(robot, blocks);

    return 0;
}