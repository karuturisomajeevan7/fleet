#pragma once
#include <string>
#include <iostream>
#include <stdexcept>

class Vehicle {
private:
    int speed;
    int temperature;
    int fuel;
    
public:
    // Constructor with validation
    Vehicle(int spd, int temp, int f) {
        if (spd < 0 || temp < 0 || f < 0) {
            throw std::invalid_argument("Vehicle parameters cannot be negative");
        }
        speed = spd;
        temperature = temp;
        fuel = f;
    }

    // Getters (const correctness)
    int getSpeed() const { return speed; }
    int getTemperature() const { return temperature; }
    int getFuel() const { return fuel; }

    // Alerts
    bool isOverheated() const { return temperature > 110; }
    bool isLowFuel() const { return fuel < 15; }

    // Display info
    void display(int id) const {
        std::cout << "Vehicle " << id 
                  << " Speed: " << speed
                  << " Temp: " << temperature
                  << " Fuel: " << fuel << std::endl;

        if (isOverheated()) std::cout << "Alert: Engine Overheating!" << std::endl;
        if (isLowFuel()) std::cout << "Warning: Low Fuel!" << std::endl;
    }
};
