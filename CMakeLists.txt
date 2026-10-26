cmake_minimum_required( VERSION 3.16 )

project( fuel_manager VERSION 1.0 )
project( fuel_manager LANGUAGES CXX )
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.17.0
)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(spdlog)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(fuel_management_system 
    src/main.cpp 
    src/sitl-sensors/fuel-sensor.cpp 
    src/sitl-sensors/air-speed-sensor.cpp
    src/event-bus/event-bus.cpp
    src/fuel-display/fuel-display-service.cpp
    src/fuel/fuel-service.cpp
    ) # assumes all cpp files are in a directory called `src`

add_executable(afrm_tests
    tests/unit/test_event_bus.cpp
    tests/unit/test_fuel_service.cpp
    tests/unit/test_fuel_display_service.cpp
    tests/integration/test_fuelService_to_fuelDisplay.cpp
    tests/integration/test_sensor_to_fuelService.cpp
    src/sitl-sensors/air-speed-sensor.cpp
    src/sitl-sensors/fuel-sensor.cpp
    src/fuel/fuel-service.cpp
    src/fuel-display/fuel-display-service.cpp
    src/event-bus/event-bus.cpp
) # testing executables

target_include_directories(
    fuel_management_system
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/.config
)
target_include_directories(
    afrm_tests 
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

target_link_libraries(afrm_tests PRIVATE GTest::gtest_main spdlog::spdlog)

include(GoogleTest)

gtest_discover_tests(afrm_tests)

target_compile_options(afrm_tests PRIVATE
    -O0
    -g
    -fprofile-instr-generate
    -fcoverage-mapping
)

target_link_options(afrm_tests PRIVATE
    -fprofile-instr-generate
)