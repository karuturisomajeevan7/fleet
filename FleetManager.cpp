/**
 * @file FleetManager.cpp
 * @brief Implementation of the FleetManager class.
 */
#include "FleetManager.h"
#include <stdexcept>
#include <cmath>

FleetManager::FleetManager() = default;

void FleetManager::addVehicle(std::unique_ptr<Vehicle> vehicle) {
    fleet_.emplace_back(std::move(vehicle));
}

double FleetManager::averageSpeed() const {
    if (fleet_.empty())
        throw std::runtime_error("Fleet is empty. Cannot compute average speed.");
    double sum = 0.0;
    for (const auto& v : fleet_)
        sum += v->getSpeed();
    return sum / fleet_.size();
}

double FleetManager::averageTemperature() const {
    if (fleet_.empty())
        throw std::runtime_error("Fleet is empty. Cannot compute average temperature.");
    double sum = 0.0;
    for (const auto& v : fleet_)
        sum += v->getTemperature();
    return sum / fleet_.size();
}

double FleetManager::averageFuel() const {
    if (fleet_.empty())
        throw std::runtime_error("Fleet is empty. Cannot compute average fuel.");
    double sum = 0.0;
    for (const auto& v : fleet_)
        sum += v->getFuel();
    return sum / fleet_.size();
}

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


const std::vector<std::unique_ptr<Vehicle>>& FleetManager::getFleet() const {
    return fleet_;
}