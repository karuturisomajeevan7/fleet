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
    void setSpeed(double speed) {
        std::lock_guard<std::mutex> lock(mtx_);
        speed_ = speed;
    }
    /**
     * @brief Sets the vehicle's temperature in a thread-safe manner.
     * @param temperature New temperature value.
     */
    void setTemperature(double temperature) {
        std::lock_guard<std::mutex> lock(mtx_);
        temperature_ = temperature;
    }
    /**
     * @brief Sets the vehicle's fuel in a thread-safe manner.
     * @param fuel New fuel value.
     */
    void setFuel(double fuel) {
        std::lock_guard<std::mutex> lock(mtx_);
        fuel_ = fuel;
    }

    /**
     * @brief Gets the vehicle's speed in a thread-safe manner.
     * @return Vehicle's speed in km/h.
     */
    double getSpeed() const override {
        std::lock_guard<std::mutex> lock(mtx_);
        return speed_;
    }
    /**
     * @brief Gets the vehicle's temperature in a thread-safe manner.
     * @return Vehicle's engine temperature in Celsius.
     */
    double getTemperature() const override {
        std::lock_guard<std::mutex> lock(mtx_);
        return temperature_;
    }
    /**
     * @brief Gets the vehicle's fuel in a thread-safe manner.
     * @return Vehicle's fuel level as a percentage (0-100).
     */
    double getFuel() const override {
        std::lock_guard<std::mutex> lock(mtx_);
        return fuel_;
    }

    mutable std::mutex mtx_;

protected:
    using Vehicle::id_;
    using Vehicle::speed_;
    using Vehicle::temperature_;
    using Vehicle::fuel_;
};

/**
 * @brief Thread-safe FleetManager for concurrent aggregation and management.
 */
class ThreadSafeFleetManager : public FleetManager {
public:
    /**
     * @brief Constructs a ThreadSafeFleetManager object.
     */
    ThreadSafeFleetManager() : FleetManager() {}

    /**
     * @brief Adds a thread-safe vehicle to the fleet.
     * @param vehicle Unique pointer to a ThreadSafeVehicle instance.
     */
    void addVehicle(std::unique_ptr<ThreadSafeVehicle> vehicle) {
        std::lock_guard<std::mutex> lock(fleet_mtx_);
        ts_fleet_.emplace_back(std::move(vehicle));
    }

    /**
     * @brief Calculates the average speed of all vehicles in a thread-safe manner.
     * @return Average speed in km/h.
     * @throws std::runtime_error if the fleet is empty.
     */
    double averageSpeed() const {
        std::lock_guard<std::mutex> lock(fleet_mtx_);
        if (ts_fleet_.empty())
            throw std::runtime_error("Fleet is empty. Cannot compute average speed.");
        double sum = 0.0;
        for (const auto& v : ts_fleet_)
            sum += v->getSpeed();
        return sum / ts_fleet_.size();
    }

    /**
     * @brief Calculates the average temperature of all vehicles in a thread-safe manner.
     * @return Average temperature in Celsius.
     * @throws std::runtime_error if the fleet is empty.
     */
    double averageTemperature() const {
        std::lock_guard<std::mutex> lock(fleet_mtx_);
        if (ts_fleet_.empty())
            throw std::runtime_error("Fleet is empty. Cannot compute average temperature.");
        double sum = 0.0;
        for (const auto& v : ts_fleet_)
            sum += v->getTemperature();
        return sum / ts_fleet_.size();
    }

    /**
     * @brief Calculates the average fuel level of all vehicles in a thread-safe manner.
     * @return Average fuel percentage.
     * @throws std::runtime_error if the fleet is empty.
     */
    double averageFuel() const {
        std::lock_guard<std::mutex> lock(fleet_mtx_);
        if (ts_fleet_.empty())
            throw std::runtime_error("Fleet is empty. Cannot compute average fuel.");
        double sum = 0.0;
        for (const auto& v : ts_fleet_)
            sum += v->getFuel();
        return sum / ts_fleet_.size();
    }

    /**
     * @brief Returns a const reference to the thread-safe fleet.
     * @return Const reference to the vector of unique pointers to ThreadSafeVehicle.
     */
    const std::vector<std::unique_ptr<ThreadSafeVehicle>>& getFleet() const {
        return ts_fleet_;
    }

private:
    std::vector<std::unique_ptr<ThreadSafeVehicle>> ts_fleet_; ///< Vector of unique pointers to ThreadSafeVehicle objects.
    mutable std::mutex fleet_mtx_; ///< Mutex to protect the fleet vector.
};

/**
 * @brief Simulates vehicle updates in a separate thread.
 * @param v Pointer to the ThreadSafeVehicle to update.
 * @param running Atomic flag to control simulation loop.
 */
void vehicle_simulation(ThreadSafeVehicle* v, std::atomic<bool>& running) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> speed_dist(40, 120);
    std::uniform_real_distribution<> temp_dist(80, 130);
    std::uniform_real_distribution<> fuel_dist(5, 100);

    while (running.load()) {
        v->setSpeed(speed_dist(gen));
        v->setTemperature(temp_dist(gen));
        v->setFuel(fuel_dist(gen));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

/**
 * @brief Main entry point for the Fleet Management application.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit code.
 */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: FleetMonitor <vehicles.csv>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: Cannot open file " << argv[1] << "\n";
        return 1;
    }

    FleetManager manager;
    std::string line;
    size_t lineNum = 0;
    size_t skipped = 0;
    size_t loaded = 0; // Added counter

    ThreadSafeFleetManager ts_manager;

    while (std::getline(file, line)) {
        ++lineNum;
        if (line.find_first_not_of(" \t\r\n") == std::string::npos)
            continue;

        std::string errorMsg;
        auto vehicle = parseVehicleCsv(line, errorMsg);
        if (vehicle) {
            manager.addVehicle(std::make_unique<Vehicle>(
                vehicle->getId(), vehicle->getSpeed(), vehicle->getTemperature(), vehicle->getFuel()));
            ts_manager.addVehicle(std::make_unique<ThreadSafeVehicle>(
                vehicle->getId(), vehicle->getSpeed(), vehicle->getTemperature(), vehicle->getFuel()));
            ++loaded; // Increment when valid vehicle is added
        } else {
            std::cerr << "Malformed line " << lineNum << ": " << errorMsg << " [" << line << "]\n";
            ++skipped;
        }
    }

    std::cout << "Loaded " << loaded << " vehicles from file\n";

    const auto& fleet = manager.getFleet();
    const auto& ts_fleet = ts_manager.getFleet();
    if (fleet.empty() || ts_fleet.empty()) {
        std::cerr << "Error: No valid vehicle data loaded. Please check your CSV file.\n";
        if (skipped > 0)
            std::cout << "Skipped " << skipped << " malformed line(s).\n";
        return 2;
    }

    std::vector<std::thread> threads;
    std::atomic<bool> running{true};
    for (const auto& v : ts_fleet) {
        threads.emplace_back(vehicle_simulation, v.get(), std::ref(running));
    }

    using clock = std::chrono::high_resolution_clock;
    constexpr int iterations = 1000;

    auto t1 = clock::now();
    double avgSpeed = 0, avgTemp = 0, avgFuel = 0;
    for (int i = 0; i < iterations; ++i) {
        avgSpeed += manager.averageSpeed();
        avgTemp += manager.averageTemperature();
        avgFuel += manager.averageFuel();
    }
    auto t2 = clock::now();
    auto single_threaded_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    auto t3 = clock::now();
    double tsAvgSpeed = 0, tsAvgTemp = 0, tsAvgFuel = 0;
    for (int i = 0; i < iterations; ++i) {
        tsAvgSpeed += ts_manager.averageSpeed();
        tsAvgTemp += ts_manager.averageTemperature();
        tsAvgFuel += ts_manager.averageFuel();
    }
    auto t4 = clock::now();
    auto multi_threaded_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();

    running = false;
    for (auto& th : threads) th.join();

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Performance Comparison (" << iterations << " iterations):\n";
    std::cout << "  Single-threaded aggregation: " << single_threaded_ms << " ms\n";
    std::cout << "  Multi-threaded aggregation:  " << multi_threaded_ms << " ms\n\n";

    std::cout << "Sample Aggregates (multi-threaded):\n";
    std::cout << "  Average Speed: " << ts_manager.averageSpeed() << " km/h\n";
    std::cout << "  Average Temperature: " << ts_manager.averageTemperature() << " °C\n";
    std::cout << "  Average Fuel: " << ts_manager.averageFuel() << " %\n";

    return 0;
}

/*
Synchronization logic:
- Each ThreadSafeVehicle uses a mutex to protect its data members for concurrent updates and reads.
- ThreadSafeFleetManager uses a mutex to protect the fleet vector during aggregation and addition.
- Aggregation functions lock the fleet and then lock each vehicle as needed (via their getters).
- This prevents race conditions between vehicle update threads and aggregation.
*/

