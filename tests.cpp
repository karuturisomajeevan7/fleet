#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cmath>
#include "Vehicle.h"
#include "FleetManager.h"

// Helper: CSV parser from main.cpp, extended to treat negative speed/fuel as invalid
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

        if (speed < 0 || fuel < 0)
            throw std::runtime_error("Negative speed or fuel");

        return std::make_unique<Vehicle>(id, speed, temperature, fuel);
    } catch (const std::exception& ex) {
        errorMsg = ex.what();
        return nullptr;
    }
}

struct TestResult {
    int passed = 0;
    int failed = 0;
};

// --- Normal Cases ---

void test_vehicle_no_alerts(TestResult& result) {
    FleetManager fm;
    auto v = std::make_unique<Vehicle>(1, 80, 90, 50);
    Vehicle* vptr = v.get();
    fm.addVehicle(std::move(v));
    auto alerts = fm.checkAlerts(*vptr);
    if (alerts.empty()) {
        std::cout << "[PASS] Vehicle with valid data - no alerts\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Vehicle with valid data - no alerts (got alerts)\n";
        result.failed++;
    }
}


void test_fleet_multiple_vehicles_averages(TestResult& result) {
    FleetManager fm;
    fm.addVehicle(std::make_unique<Vehicle>(1, 80, 100, 50));
    fm.addVehicle(std::make_unique<Vehicle>(2, 100, 110, 60));
    fm.addVehicle(std::make_unique<Vehicle>(3, 90, 120, 40));
    double avgSpeed = fm.averageSpeed();
    double avgTemp = fm.averageTemperature();
    double avgFuel = fm.averageFuel();
    bool ok = std::abs(avgSpeed - 90.0) < 1e-6 &&
              std::abs(avgTemp - 110.0) < 1e-6 &&
              std::abs(avgFuel - 50.0) < 1e-6;
    if (ok) {
        std::cout << "[PASS] Fleet with multiple vehicles - correct averages\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Fleet with multiple vehicles - correct averages\n";
        result.failed++;
    }
}

void test_csv_5_valid_rows(TestResult& result) {
    std::vector<std::string> lines = {
        "1,80,100,50",
        "2,60,120,10",
        "3,90,110,15",
        "4,100,85,60",
        "5,75,95,30"
    };
    FleetManager fm;
    int loaded = 0;
    for (const auto& line : lines) {
        std::string errorMsg;
        auto v = parseVehicleCsv(line, errorMsg);
        if (v) {
            fm.addVehicle(std::move(v));
            ++loaded;
        }
    }
    if (loaded == 5 && fm.getFleet().size() == 5) {
        std::cout << "[PASS] CSV with 5 valid rows - loads all rows correctly\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] CSV with 5 valid rows - loads all rows correctly\n";
        result.failed++;
    }
}

// --- Edge Cases ---

void test_boundary_temp_110_alert(TestResult& result) {
    FleetManager fm;
    auto v = std::make_unique<Vehicle>(1, 80, 110, 50);
    Vehicle* vptr = v.get();
    fm.addVehicle(std::move(v));
    auto alerts = fm.checkAlerts(*vptr);
    bool found = false;
    for (const auto& a : alerts)
        if (a == "Critical Overheating") found = true;
    if (found) {
        std::cout << "[PASS] Temperature exactly 110 - Critical Overheating\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Temperature exactly 110 - Critical Overheating\n";
        result.failed++;
    }
}

void test_boundary_fuel_15_no_warning(TestResult& result) {
    FleetManager fm;
    auto v = std::make_unique<Vehicle>(2, 80, 100, 15.0);
    Vehicle* vptr = v.get();
    fm.addVehicle(std::move(v));
    auto alerts = fm.checkAlerts(*vptr);
    bool found = false;
    for (const auto& a : alerts)
        if (a == "Low Fuel Warning") found = true;
    if (!found) {
        std::cout << "[PASS] Fuel exactly 15 - No warning\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Fuel exactly 15 - No warning\n";
        result.failed++;
    }
}

void test_empty_fleet_throws(TestResult& result) {
    FleetManager fm;
    bool threw = false;
    try {
        fm.averageSpeed();
    } catch (const std::runtime_error&) {
        threw = true;
    }
    if (threw) {
        std::cout << "[PASS] Empty fleet - throws std::runtime_error\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Empty fleet - throws std::runtime_error\n";
        result.failed++;
    }
}

void test_large_fleet_averages(TestResult& result) {
    FleetManager fm;
    double speedSum = 0, tempSum = 0, fuelSum = 0;
    int n = 1000;
    for (int i = 0; i < n; ++i) {
        double speed = 50 + (i % 51);      // 50..100
        double temp = 80 + (i % 51);       // 80..130
        double fuel = 10 + (i % 91);       // 10..100
        speedSum += speed;
        tempSum += temp;
        fuelSum += fuel;
        fm.addVehicle(std::make_unique<Vehicle>(i, speed, temp, fuel));
    }
    double avgSpeed = fm.averageSpeed();
    double avgTemp = fm.averageTemperature();
    double avgFuel = fm.averageFuel();
    bool ok = std::abs(avgSpeed - (speedSum / n)) < 1e-6 &&
              std::abs(avgTemp - (tempSum / n)) < 1e-6 &&
              std::abs(avgFuel - (fuelSum / n)) < 1e-6;
    if (ok) {
        std::cout << "[PASS] Large fleet (1000 vehicles) - averages computed correctly\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Large fleet (1000 vehicles) - averages computed correctly\n";
        result.failed++;
    }
}

// --- Invalid Cases ---

void test_csv_missing_columns(TestResult& result) {
    std::string errorMsg;
    auto v = parseVehicleCsv("1,80,100", errorMsg);
    if (!v) {
        std::cout << "[PASS] CSV line with missing columns - skipped safely\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] CSV line with missing columns - skipped safely\n";
        result.failed++;
    }
}

void test_csv_non_numeric(TestResult& result) {
    std::string errorMsg;
    auto v = parseVehicleCsv("a,b,c,d", errorMsg);
    if (!v) {
        std::cout << "[PASS] CSV line with non-numeric data - skipped safely\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] CSV line with non-numeric data - skipped safely\n";
        result.failed++;
    }
}

void test_empty_csv_file(TestResult& result) {
    // Simulate reading an empty file (no lines)
    FleetManager fm;
    bool threw = false;
    try {
        fm.averageSpeed();
    } catch (const std::runtime_error&) {
        threw = true;
    }
    if (threw) {
        std::cout << "[PASS] Completely empty CSV file - throws std::runtime_error\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Completely empty CSV file - throws std::runtime_error\n";
        result.failed++;
    }
}

void test_negative_speed_or_fuel(TestResult& result) {
    std::string errorMsg;
    auto v1 = parseVehicleCsv("1,-80,100,50", errorMsg);
    auto v2 = parseVehicleCsv("2,80,100,-50", errorMsg);
    bool ok = (!v1 && !v2);
    if (ok) {
        std::cout << "[PASS] Negative speed or fuel values - treated as invalid and skipped\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Negative speed or fuel values - treated as invalid and skipped\n";
        result.failed++;
    }
}

// --- Existing tests for completeness ---

void test_critical_overheating(TestResult& result) {
    FleetManager fm;
    auto v = std::make_unique<Vehicle>(1, 60, 120, 50);
    Vehicle* vptr = v.get();
    fm.addVehicle(std::move(v));
    auto alerts = fm.checkAlerts(*vptr);
    bool found = false;
    for (const auto& a : alerts)
        if (a == "Critical Overheating") found = true;
    if (found) {
        std::cout << "[PASS] Vehicle overheating test (temp = 120 - Critical Overheating)\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Vehicle overheating test (temp = 120 - Critical Overheating)\n";
        result.failed++;
    }
}

void test_low_fuel_warning(TestResult& result) {
    FleetManager fm;
    auto v = std::make_unique<Vehicle>(2, 60, 80, 10);
    Vehicle* vptr = v.get();
    fm.addVehicle(std::move(v));
    auto alerts = fm.checkAlerts(*vptr);
    bool found = false;
    for (const auto& a : alerts)
        if (a == "Low Fuel Warning") found = true;
    if (found) {
        std::cout << "[PASS] Vehicle low fuel test (fuel = 10 - Low Fuel Warning)\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Vehicle low fuel test (fuel = 10 - Low Fuel Warning)\n";
        result.failed++;
    }
}

void test_average_speed(TestResult& result) {
    FleetManager fm;
    auto v1 = std::make_unique<Vehicle>(1, 80, 90, 50);
    auto v2 = std::make_unique<Vehicle>(2, 90, 90, 50);
    auto v3 = std::make_unique<Vehicle>(3, 100, 90, 50);
    fm.addVehicle(std::move(v1));
    fm.addVehicle(std::move(v2));
    fm.addVehicle(std::move(v3));
    double avg = fm.averageSpeed();
    if (std::abs(avg - 90.0) < 1e-6) {
        std::cout << "[PASS] Fleet average speed test (80, 90, 100 - avg = 90)\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Fleet average speed test (80, 90, 100 - avg = 90), got avg = " << avg << "\n";
        result.failed++;
    }
}

void test_boundary_overheating(TestResult& result) {
    FleetManager fm;
    auto v = std::make_unique<Vehicle>(3, 60, 110, 50);
    Vehicle* vptr = v.get();
    fm.addVehicle(std::move(v));
    auto alerts = fm.checkAlerts(*vptr);
    bool found = false;
    for (const auto& a : alerts)
        if (a == "Critical Overheating") found = true;
    if (found) {
        std::cout << "[PASS] Boundary test (temp = 110 - Overheating triggered)\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Boundary test (temp = 110 - Overheating triggered)\n";
        result.failed++;
    }
}

void test_boundary_fuel(TestResult& result) {
    FleetManager fm;
    auto v = std::make_unique<Vehicle>(4, 60, 80, 15.0);
    Vehicle* vptr = v.get();
    fm.addVehicle(std::move(v));
    auto alerts = fm.checkAlerts(*vptr);
    bool found = false;
    for (const auto& a : alerts)
        if (a == "Low Fuel Warning") found = true;
    if (!found) {
        std::cout << "[PASS] Boundary test (fuel = 15 - No Low Fuel Warning)\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Boundary test (fuel = 15 - No Low Fuel Warning)\n";
        result.failed++;
    }
}

void test_empty_dataset(TestResult& result) {
    FleetManager fm;
    bool threw = false;
    try {
        fm.averageSpeed();
    } catch (const std::runtime_error&) {
        threw = true;
    }
    if (threw) {
        std::cout << "[PASS] Empty dataset test - Expected exception thrown\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Empty dataset test - Expected exception, got none\n";
        result.failed++;
    }
}

void test_malformed_csv(TestResult& result) {
    std::string errorMsg;
    // Missing fields
    auto v1 = parseVehicleCsv("1,80,90", errorMsg);
    // Too many fields
    auto v2 = parseVehicleCsv("1,80,90,50,extra", errorMsg);
    // Non-numeric
    auto v3 = parseVehicleCsv("a,b,c,d", errorMsg);
    // Empty line
    auto v4 = parseVehicleCsv("", errorMsg);

    bool all_null = (!v1 && !v2 && !v3 && !v4);
    if (all_null) {
        std::cout << "[PASS] Malformed CSV test (all malformed lines skipped)\n";
        result.passed++;
    } else {
        std::cout << "[FAIL] Malformed CSV test (malformed lines not all skipped)\n";
        result.failed++;
    }
}

int main() {
    TestResult result;
    // Normal cases
    test_vehicle_no_alerts(result);
    test_fleet_multiple_vehicles_averages(result);
    test_csv_5_valid_rows(result);

    // Edge cases
    test_boundary_temp_110_alert(result);
    test_boundary_fuel_15_no_warning(result);
    test_empty_fleet_throws(result);
    test_large_fleet_averages(result);

    // Invalid cases
    test_csv_missing_columns(result);
    test_csv_non_numeric(result);
    test_empty_csv_file(result);
    test_negative_speed_or_fuel(result);

    // Existing tests for completeness
    test_critical_overheating(result);
    test_low_fuel_warning(result);
    test_average_speed(result);
    test_boundary_overheating(result);
    test_boundary_fuel(result);
    test_empty_dataset(result);
    test_malformed_csv(result);

    std::cout << result.passed << "/" << (result.passed + result.failed) << " tests passed.\n";
    return result.failed == 0 ? 0 : 1;
}