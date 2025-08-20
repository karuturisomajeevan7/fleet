cmake_minimum_required(VERSION 3.10)
project(FleetMonitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(FleetMonitor
    main.cpp
    Vehicle.cpp
    FleetManager.cpp
)

target_include_directories(FleetMonitor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


# Copy vehicles.csv to the build directory after build
add_custom_command(
    TARGET FleetMonitor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/vehicles.csv
            $<TARGET_FILE_DIR:FleetMonitor>
)