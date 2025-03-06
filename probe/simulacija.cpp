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

using namespace std;

using json = nlohmann::json;

int main() {
    // Putanja do JSON datoteke
    string json_file_path = "C:/Users/amrad/OneDrive/Documents/LEGO Education SPIKE/project.json";

    // Otvorite JSON datoteku
    ifstream file(json_file_path);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return 1;
    }

    // Parsirajte JSON datoteku
    json j;
    file >> j;

    // Pristupite podacima
    cout << "Project Name: " << j["targets"][1]["name"] << std::endl;

    // Pristupite blokovima
    auto blocks = j["targets"][1]["blocks"];
    for (auto& [key, value] : blocks.items()) {
        cout << "Block ID: " << key << ", Opcode: " << value["opcode"] << std::endl;
    }

    return 0;
}