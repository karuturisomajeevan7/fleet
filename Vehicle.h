#pragma once
#include <string>

/**
 * @file Vehicle.h
 * @brief Declaration of the Vehicle class representing a vehicle's telemetry data.
 */

/**
 * @brief Represents a vehicle with telemetry data.
 */
class Vehicle {
public:

    /**
     * @brief Constructs a Vehicle object.
     * @param id Vehicle's unique identifier.
     * @param speed Vehicle's speed in km/h.
     * @param temperature Vehicle's engine temperature in Celsius.
     * @param fuel Vehicle's fuel level as a percentage (0-100).
     */
    Vehicle(int id, double speed, double temperature, double fuel);

    /**
     * @brief Gets the vehicle's unique identifier.
     * @return Vehicle's unique identifier.
     */
    int getId() const;

    /**
     * @brief Gets the vehicle's speed.
     * @return Vehicle's speed in km/h.
     */
    virtual double getSpeed() const;

    /**
     * @brief Gets the vehicle's engine temperature.
     * @return Vehicle's engine temperature in Celsius.
     */
    virtual double getTemperature() const;

    /**
     * @brief Gets the vehicle's fuel level.
     * @return Vehicle's fuel level as a percentage (0-100).
     */
    virtual double getFuel() const;

protected:
    int id_;             ///< Vehicle's unique identifier.
    double speed_;       ///< Vehicle's speed in km/h.
    double temperature_; ///< Vehicle's engine temperature in Celsius.
    double fuel_;        ///< Vehicle's fuel level as a percentage (0-100).
};