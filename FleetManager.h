#pragma once
#include "Vehicle.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @file FleetManager.h
 * @brief Declaration of the FleetManager class for managing a fleet of vehicles.
 */

/**
 * @brief Manages a fleet of vehicles, computes statistics, and triggers alerts.
 */
class FleetManager {
public:
    /**
     * @brief Constructs a FleetManager object.
     */
    FleetManager();

    /**
     * @brief Adds a vehicle to the fleet.
     * @param vehicle Unique pointer to a Vehicle instance.
     */
    void addVehicle(std::unique_ptr<Vehicle> vehicle);

    /**
     * @brief Calculates the average speed of all vehicles.
     * @return Average speed in km/h.
     * @throws std::runtime_error if the fleet is empty.
     */
    double averageSpeed() const;

    /**
     * @brief Calculates the average temperature of all vehicles.
     * @return Average temperature in Celsius.
     * @throws std::runtime_error if the fleet is empty.
     */
    double averageTemperature() const;

    /**
     * @brief Calculates the average fuel level of all vehicles.
     * @return Average fuel percentage.
     * @throws std::runtime_error if the fleet is empty.
     */
    double averageFuel() const;

    /**
     * @brief Checks for alerts for a given vehicle.
     * @param vehicle Reference to a Vehicle.
     * @return Vector of alert messages (empty if none).
     */
    std::vector<std::string> checkAlerts(const Vehicle& vehicle) const;

    /**
     * @brief Returns a const reference to the fleet.
     * @return Const reference to the vector of unique pointers to Vehicle.
     */
    const std::vector<std::unique_ptr<Vehicle>>& getFleet() const;

    
private:
    std::vector<std::unique_ptr<Vehicle>> fleet_; ///< Vector of unique pointers to Vehicle objects.
};