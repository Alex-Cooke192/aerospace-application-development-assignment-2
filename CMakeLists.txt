cmake_minimum_required( VERSION 3.16 )

project( fuel_manager VERSION 1.0 )
project( fuel_manager LANGUAGES CXX )
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.3
)

FetchContent_MakeAvailable(spdlog)

add_executable(fuel_management_system 
    src/main.cpp 
    src/sitl-sensors/fuel-sensor.cpp 
    src/sitl-sensors/air-speed-sensor.cpp
    src/event-bus/event-bus.cpp
    src/fuel-display/fuel-display-service.cpp
    src/fuel/fuel-service.cpp
    ) # assumes all cpp files are in a directory called `src`

target_include_directories(
    fuel_management_system
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/.config
)

message(STATUS "C Compiler:   ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

target_compile_features( fuel_management_system PUBLIC cxx_std_26 ) # sets the C++ standard to C++26, as agreed in design report. 

target_link_libraries(
    fuel_management_system
    PRIVATE spdlog::spdlog
)