#ifndef STRING_CONVERSION_HPP
#define STRING_CONVERSION_HPP

#include <string>
#include <cctype>
using namespace std;

double parseDouble(const string& str) {
    if (str.empty()) return 0.0;
    
    try {
        // Ako je samo "e" ili "eee..." onda je beskonačno
        if (str.find_first_not_of("eE") == string::npos) {
            return numeric_limits<double>::infinity();
        }
        
        // Inače koristi standardnu notaciju (npr. 2e3 = 200)
        return stod(str);
    } catch (...) {
        return 0.0;  // Ako parsing padne, vrati 0
    }
}

int parseInt(const string& str) {
    if (str.empty()) return 0;
    
    try {
        // Ako je samo "e" ili "eee..." onda je beskonačno
        if (str.find_first_not_of("eE") == string::npos) {
            return numeric_limits<int>::max();
        }
        
        // Inače koristi standardnu notaciju
        return stoi(str);
    } catch (...) {
        return 0;  // Ako parsing padne, vrati 0
    }
}

#endif