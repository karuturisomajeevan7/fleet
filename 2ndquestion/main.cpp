#include "FleetManager.h"
#include <iostream>

int main() {
    FleetManager fleet;
    
    try {
        fleet.loadFromFile("vehicles.csv");  // Example CSV: speed,temp,fuel
        fleet.displayAll();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
