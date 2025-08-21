/**
 * @file FleetManager.cpp
 * @brief Implementation of the FleetManager class.
 */

#include "FleetManager.h"
#include <stdexcept>
#include <cmath>

/**
 * @brief Constructs a FleetManager object.
 */
FleetManager::FleetManager() = default;

/**
 * @brief Adds a vehicle to the fleet.
 * @param vehicle Unique pointer to a Vehicle instance.
 */
void FleetManager::addVehicle(std::unique_ptr<Vehicle> vehicle) {
    fleet_.emplace_back(std::move(vehicle));
}

/**
 * @brief Calculates the average speed of all vehicles.
 * @return Average speed in km/h.
 * @throws std::runtime_error if the fleet is empty.
 */
double FleetManager::averageSpeed() const {
    if (fleet_.empty())
        throw std::runtime_error("Fleet is empty. Cannot compute average speed.");
    double sum = 0.0;
    for (const auto& v : fleet_)
        sum += v->getSpeed();
    return sum / fleet_.size();
}

/**
 * @brief Calculates the average temperature of all vehicles.
 * @return Average temperature in Celsius.
 * @throws std::runtime_error if the fleet is empty.
 */
double FleetManager::averageTemperature() const {
    if (fleet_.empty())
        throw std::runtime_error("Fleet is empty. Cannot compute average temperature.");
    double sum = 0.0;
    for (const auto& v : fleet_)
        sum += v->getTemperature();
    return sum / fleet_.size();
}

/**
 * @brief Calculates the average fuel level of all vehicles.
 * @return Average fuel percentage.
 * @throws std::runtime_error if the fleet is empty.
 */
double FleetManager::averageFuel() const {
    if (fleet_.empty())
        throw std::runtime_error("Fleet is empty. Cannot compute average fuel.");
    double sum = 0.0;
    for (const auto& v : fleet_)
        sum += v->getFuel();
    return sum / fleet_.size();
}

/**
 * @brief Checks for alerts for a given vehicle.
 * @param vehicle Reference to a Vehicle.
 * @return Vector of alert messages (empty if none).
 */
std::vector<std::string> FleetManager::checkAlerts(const Vehicle& vehicle) const {
    std::vector<std::string> alerts;
    constexpr double EPSILON = 1e-6;
    // Boundary: exactly 110°C triggers alert
    if (vehicle.getTemperature() > 110.0 - EPSILON)
        alerts.emplace_back("Critical Overheating");
    // Boundary: exactly 15% fuel is OK, <15 triggers alert
    if (vehicle.getFuel() < 15.0 - EPSILON)
        alerts.emplace_back("Low Fuel Warning");
    return alerts;
}

/**
 * @brief Returns a const reference to the fleet.
 * @return Const reference to the vector of unique pointers to Vehicle.
 */
const std::vector<std::unique_ptr<Vehicle>>& FleetManager::getFleet() const {
    return fleet_;
}