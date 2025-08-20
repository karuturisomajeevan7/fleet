#pragma once
#include "Vehicle.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

class FleetManager {
private:
    std::vector<Vehicle> vehicles;
public:
    // Load vehicles from CSV file
    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        std::string line;
        int lineNumber = 0;

        while (std::getline(file, line)) {
            ++lineNumber;
            std::stringstream ss(line);
            int speed, temp, fuel;
            char comma;

            if (!(ss >> speed >> comma >> temp >> comma >> fuel)) {
                std::cerr << "Skipping invalid line " << lineNumber << ": " << line << std::endl;
                continue;
            }

            try {
                vehicles.emplace_back(speed, temp, fuel);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Skipping invalid data on line " << lineNumber << ": " << e.what() << std::endl;
            }
        }
    }

    // Display all vehicles
    void displayAll() const {
        for (size_t i = 0; i < vehicles.size(); ++i) {
            vehicles[i].display(i);
        }
    }
};
