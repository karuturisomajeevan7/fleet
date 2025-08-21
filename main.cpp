/**
 * @file main.cpp
 * @brief Entry point for the Fleet Management application. Handles CSV parsing, concurrency simulation, and performance comparison.
 */

#include "FleetManager.h"
#include "Vehicle.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <iomanip>
#include <thread>
#include <vector>
#include <random>
#include <mutex>
#include <chrono>
#include <atomic>

/**
 * @brief Parses a CSV line into a Vehicle object.
 * @param line CSV line.
 * @param[out] errorMsg Error message if parsing fails.
 * @return unique_ptr to Vehicle if successful, nullptr otherwise.
 */
std::unique_ptr<Vehicle> parseVehicleCsv(const std::string& line, std::string& errorMsg) {
    std::istringstream ss(line);
    std::string token;
    int id;
    double speed, temperature, fuel;

    try {
        if (!std::getline(ss, token, ',')) throw std::runtime_error("Missing ID");
        id = std::stoi(token);

        if (!std::getline(ss, token, ',')) throw std::runtime_error("Missing speed");
        speed = std::stod(token);

        if (!std::getline(ss, token, ',')) throw std::runtime_error("Missing temperature");
        temperature = std::stod(token);

        if (!std::getline(ss, token, ',')) throw std::runtime_error("Missing fuel");
        fuel = std::stod(token);

        if (std::getline(ss, token, ',')) throw std::runtime_error("Too many fields");

        return std::make_unique<Vehicle>(id, speed, temperature, fuel);
    } catch (const std::exception& ex) {
        errorMsg = ex.what();
        return nullptr;
    }
}

// --- Concurrency additions ---

/**
 * @class ThreadSafeVehicle
 * @brief Thread-safe wrapper for Vehicle, allowing concurrent updates and reads.
 */
class ThreadSafeVehicle : public Vehicle {
public:
    /**
     * @brief Constructs a ThreadSafeVehicle object.
     * @param id Vehicle's unique identifier.
     * @param speed Vehicle's speed in km/h.
     * @param temperature Vehicle's engine temperature in Celsius.
     * @param fuel Vehicle's fuel level as a percentage (0-100).
     */
    ThreadSafeVehicle(int id, double speed, double temperature, double fuel)
        : Vehicle(id, speed, temperature, fuel) {}

    /**
     * @brief Sets the vehicle's speed in a thread-safe manner.
     * @param speed New speed value.
     */
    void setSpeed(double speed);

    /**
     * @brief Sets the vehicle's temperature in a thread-safe manner.
     * @param temperature New temperature value.
     */
    void setTemperature(double temperature);

    /**
     * @brief Sets the vehicle's fuel in a thread-safe manner.
     * @param fuel New fuel value.
     */
    void setFuel(double fuel);

    /**
     * @brief Gets the vehicle's speed in a thread-safe manner.
     * @return Vehicle's speed in km/h.
     */
    double getSpeed() const override;

    /**
     * @brief Gets the vehicle's temperature in a thread-safe manner.
     * @return Vehicle's engine temperature in Celsius.
     */
    double getTemperature() const override;

    /**
     * @brief Gets the vehicle's fuel in a thread-safe manner.
     * @return Vehicle's fuel level as a percentage (0-100).
     */
    double getFuel() const override;

    mutable std::mutex mtx_;

protected:
    using Vehicle::id_;
    using Vehicle::speed_;
    using Vehicle::temperature_;
    using Vehicle::fuel_;
};

/**
 * @class ThreadSafeFleetManager
 * @brief Thread-safe FleetManager for concurrent aggregation and management.
 */
class ThreadSafeFleetManager : public FleetManager {
public:
    /**
     * @brief Constructs a ThreadSafeFleetManager object.
     */
    ThreadSafeFleetManager();

    /**
     * @brief Adds a thread-safe vehicle to the fleet.
     * @param vehicle Unique pointer to a ThreadSafeVehicle instance.
     */
    void addVehicle(std::unique_ptr<ThreadSafeVehicle> vehicle);

    /**
     * @brief Calculates the average speed of all vehicles in a thread-safe manner.
     * @return Average speed in km/h.
     * @throws std::runtime_error if the fleet is empty.
     */
    double averageSpeed() const;

    /**
     * @brief Calculates the average temperature of all vehicles in a thread-safe manner.
     * @return Average temperature in Celsius.
     * @throws std::runtime_error if the fleet is empty.
     */
    double averageTemperature() const;

    /**
     * @brief Calculates the average fuel level of all vehicles in a thread-safe manner.
     * @return Average fuel percentage.
     * @throws std::runtime_error if the fleet is empty.
     */
    double averageFuel() const;

    /**
     * @brief Returns a const reference to the thread-safe fleet.
     * @return Const reference to the vector of unique pointers to ThreadSafeVehicle.
     */
    const std::vector<std::unique_ptr<ThreadSafeVehicle>>& getFleet() const;

private:
    std::vector<std::unique_ptr<ThreadSafeVehicle>> ts_fleet_; ///< Vector of unique pointers to ThreadSafeVehicle objects.
    mutable std::mutex fleet_mtx_; ///< Mutex to protect the fleet vector.
};

/**
 * @brief Simulates vehicle updates in a separate thread.
 * @param v Pointer to the ThreadSafeVehicle to update.
 * @param running Atomic flag to control simulation loop.
 */
void vehicle_simulation(ThreadSafeVehicle* v, std::atomic<bool>& running);

/**
 * @brief Main entry point for the Fleet Management application.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit code.
 */
int main(int argc, char* argv[]);

/*
 * @brief Synchronization logic:
 * - Each ThreadSafeVehicle uses a mutex to protect its data members for concurrent updates and reads.
 * - ThreadSafeFleetManager uses a mutex to protect the fleet vector during aggregation and addition.
 * - Aggregation functions lock the fleet and then lock each vehicle as needed (via their getters).
 * - This prevents race conditions between vehicle update threads and aggregation.
 */

