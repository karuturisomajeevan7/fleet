/**
 * @file Vehicle.cpp
 * @brief Implementation of the Vehicle class.
 */
#include "Vehicle.h"

Vehicle::Vehicle(int id, double speed, double temperature, double fuel)
    : id_(id), speed_(speed), temperature_(temperature), fuel_(fuel) {}

int Vehicle::getId() const { return id_; }

double Vehicle::getSpeed() const { return speed_; }

double Vehicle::getTemperature() const { return temperature_; }


double Vehicle::getFuel() const { return fuel_; }