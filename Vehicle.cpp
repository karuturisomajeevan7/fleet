/**
 * @file Vehicle.cpp
 * @brief Implementation of the Vehicle class.
 */

#include "Vehicle.h"

/**
 * @brief Constructs a Vehicle object.
 * @param id Vehicle's unique identifier.
 * @param speed Vehicle's speed in km/h.
 * @param temperature Vehicle's engine temperature in Celsius.
 * @param fuel Vehicle's fuel level as a percentage (0-100).
 */
Vehicle::Vehicle(int id, double speed, double temperature, double fuel)
    : id_(id), speed_(speed), temperature_(temperature), fuel_(fuel) {}

/**
 * @brief Gets the vehicle's unique identifier.
 * @return Vehicle's unique identifier.
 */
int Vehicle::getId() const { return id_; }

/**
 * @brief Gets the vehicle's speed.
 * @return Vehicle's speed in km/h.
 */
double Vehicle::getSpeed() const { return speed_; }

/**
 * @brief Gets the vehicle's engine temperature.
 * @return Vehicle's engine temperature in Celsius.
 */
double Vehicle::getTemperature() const { return temperature_; }

/**
 * @brief Gets the vehicle's fuel level.
 * @return Vehicle's fuel level as a percentage (0-100).
 */
double Vehicle::getFuel() const { return fuel_; }