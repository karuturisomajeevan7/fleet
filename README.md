# Fleet Management System

## Introduction

This project is a beginner-friendly C++ fleet management system that demonstrates:
- **CSV parsing** for vehicle data input,
- **Vehicle telemetry management** (speed, temperature, fuel),
- **Fleet-wide statistics** (average speed, temperature, fuel),
- **Alerting** for critical overheating and low fuel conditions,
- **Unit and integration testing**,
- **Optional concurrency demo** (multi-threaded vehicle updates).

The system loads vehicle data from a CSV file, computes statistics, and prints alerts for vehicles that exceed safe thresholds.


---

## System Requirements

- **C++17 compiler** (e.g., g++ 7.0 or newer)
- **Visual Studio Code** (recommended) with the C++ extension
- Windows, Linux, or macOS

---

## Project Files

| File                | Description                                                      |
|---------------------|------------------------------------------------------------------|
| `main.cpp`          | Application entry point. Loads CSV, manages fleet, prints output.|
| `Vehicle.h/cpp`     | Defines the `Vehicle` class (id, speed, temperature, fuel).      |
| `FleetManager.h/cpp`| Manages a fleet of vehicles, computes averages, checks alerts.   |
| `tests.cpp`         | Unit and integration tests for all core logic.                   |
| `vehicles.csv`      | Sample input data file (CSV format).                             |
| `CMakeLists.txt`    | CMake build script (optional, for multi-platform builds).        |

---

## How to Build

Open a terminal in your project directory and run:

**Build the main application:**
```sh
g++ -std=c++17 -Wall -Wextra -O2 main.cpp Vehicle.cpp FleetManager.cpp -o fleet_app
```

**Build the tests:**
```sh
g++ -std=c++17 -Wall -Wextra -O2 tests.cpp Vehicle.cpp FleetManager.cpp -o fleet_tests
```

If you use CMake:
```sh
cmake -S . -B build
cmake --build build
```
The executables will be in the `build` directory.

---

## Sample Input/Output

### Sample `vehicles.csv`
```
id,speed,temperature,fuel
1,80,100,50
2,60,120,10
3,90,110,15
4,100,85,60
5,75,95,30
```

### Running the Application

```sh
./fleet_app vehicles.csv
```

**Example Output:**
```
Loaded 5 vehicles from file
Performance Comparison (1000 iterations):
  Single-threaded aggregation: 12 ms
  Multi-threaded aggregation:  15 ms

Sample Aggregates (multi-threaded):
  Average Speed: 81.00 km/h
  Average Temperature: 102.00 °C
  Average Fuel: 33.00 %

Vehicle Alerts:
  Vehicle ID 2: Critical Overheating, Low Fuel Warning
  Vehicle ID 3: Critical Overheating
```

---

### Running the Tests

```sh
./fleet_tests
```

**Example Output:**
```
[PASS] Vehicle with valid data → no alerts
[PASS] Fleet with multiple vehicles → correct averages
[PASS] CSV with 5 valid rows → loads all rows correctly
[PASS] Temperature exactly 110 → 'Critical Overheating'
[PASS] Fuel exactly 15 → No warning
[PASS] Empty fleet → throws std::runtime_error
[PASS] Large fleet (1000 vehicles) → averages computed correctly
[PASS] CSV line with missing columns → skipped safely
[PASS] CSV line with non-numeric data → skipped safely
[PASS] Completely empty CSV file → throws std::runtime_error
[PASS] Negative speed or fuel values → treated as invalid and skipped
...
16/16 tests passed.
```

---

## Notes

- Place `vehicles.csv` in the same directory as your executable or configure your build system to copy it there.
- The program skips malformed or empty lines and logs errors for them.
- Alerts:
  - **Critical Overheating**: Triggered if temperature ≥ 110°C.
  - **Low Fuel Warning**: Triggered if fuel < 15%.
- The code is modular and easy to extend for more vehicle attributes or alert types.

---

## License

This project is provided for educational