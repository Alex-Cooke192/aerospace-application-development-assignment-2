#include <gtest/gtest.h>
#include <sstream>
#include <unistd.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <iterator>

#include "end_to_end_integration_test_data.h"
#include "event-bus/event-bus.h"
#include "event-bus/events.h"
#include "fuel/fuel-service.h"
#include "sitl-sensors/air-speed-sensor.h"
#include "sitl-sensors/fuel-sensor.h"
#include "fuel-display/fuel-display-service.h"
#include "sensor-processing/sensor-processing-service.h"

TEST(EndToEndIntegrationTest, ValidSensorReadingsUpdatesFuelState) {
    EventBus bus;
    FuelService fuelService(bus, TestConfig());
    FuelDisplayService fuelDisplayService(bus);
    SensorProcessingService sensorProcessingService(bus, TestConfig());

    int fuelLevelEventCount = 0, fuelConsumptionEventCount = 0, airSpeedEventCount = 0, fuelRangeEventCount = 0;
    float testFuelLevel, testFuelConsumption, testAirSpeed, testFuelRange; 
    bus.subscribe<FuelLevelChanged>([&](const auto& event) { testFuelLevel = event.New_Fuel_Level; ++fuelLevelEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<FuelConsumptionChanged>([&](const auto& event) { testFuelConsumption = event.New_Fuel_Consumption; ++fuelConsumptionEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<AirSpeedChanged>([&](const auto& event) { testAirSpeed = event.New_Air_Speed; ++airSpeedEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<FuelRangeChanged>([&](const auto& event) { testFuelRange = event.New_Fuel_Range; ++fuelRangeEventCount; fuelService.SetFuelRangeFlag(false); });
    std::ostringstream oss; // Set up oss to capture output stream
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    std::thread sensorThread(validLoop, std::ref(bus)); 
    sensorThread.join();
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    float calculatedFuelRange = (198.5f/0.0321f)*251.5f;
    EXPECT_EQ(fuelLevelEventCount, 6);
    EXPECT_EQ(fuelConsumptionEventCount, 6);
    EXPECT_EQ(airSpeedEventCount, 2);
    EXPECT_EQ(fuelRangeEventCount, 14);
    EXPECT_EQ(testFuelLevel, 198.5f); 
    EXPECT_EQ(testFuelConsumption, 0.0321f);
    EXPECT_EQ(testAirSpeed, 251.5f);
    EXPECT_NEAR(testFuelRange, calculatedFuelRange, 1.0f); 
}

TEST(EndToEndIntegrationTest, AnomalousSensorReadingsUpdatesFuelState) {
    EventBus bus;
    FuelService fuelService(bus, TestConfig());
    FuelDisplayService fuelDisplayService(bus);
    SensorProcessingService sensorProcessingService(bus, TestConfig());

    int fuelLevelEventCount = 0, fuelConsumptionEventCount = 0, airSpeedEventCount = 0, fuelRangeEventCount = 0;
    float testFuelLevel, testFuelConsumption, testAirSpeed, testFuelRange; 
    bus.subscribe<FuelLevelChanged>([&](const auto& event) { testFuelLevel = event.New_Fuel_Level; ++fuelLevelEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<FuelConsumptionChanged>([&](const auto& event) { testFuelConsumption = event.New_Fuel_Consumption; ++fuelConsumptionEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<AirSpeedChanged>([&](const auto& event) { testAirSpeed = event.New_Air_Speed; ++airSpeedEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<FuelRangeChanged>([&](const auto& event) { testFuelRange = event.New_Fuel_Range; ++fuelRangeEventCount; fuelService.SetFuelRangeFlag(false); });
    std::ostringstream oss; // Set up oss to capture output stream
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    std::thread sensorThread(anomalousLoop, std::ref(bus)); 
    sensorThread.join();
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    float calculatedFuelRange = (198.0f/0.0320f)*252.0f;
    EXPECT_EQ(fuelLevelEventCount, 2);
    EXPECT_EQ(fuelConsumptionEventCount, 2);
    EXPECT_EQ(airSpeedEventCount, 2);
    EXPECT_EQ(fuelRangeEventCount, 6);
    EXPECT_EQ(testFuelLevel, 198.0f); 
    EXPECT_EQ(testFuelConsumption, 0.0320f);
    EXPECT_EQ(testAirSpeed, 252.0f);
    EXPECT_NEAR(testFuelRange, calculatedFuelRange, 1.0f); 
}

TEST(EndToEndIntegrationTest, InvalidSensorReadingsDoesNotUpdateFuelState) {
    EventBus bus;
    FuelService fuelService(bus, TestConfig());
    FuelDisplayService fuelDisplayService(bus);
    SensorProcessingService sensorProcessingService(bus, TestConfig());

    int fuelLevelEventCount = 0, fuelConsumptionEventCount = 0, airSpeedEventCount = 0, fuelRangeEventCount = 0;
    float testFuelLevel = 0.0, testFuelConsumption = 0.0, testAirSpeed = 0.0, testFuelRange = 0.0; 
    bus.subscribe<FuelLevelChanged>([&](const auto& event) { testFuelLevel = event.New_Fuel_Level; ++fuelLevelEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<FuelConsumptionChanged>([&](const auto& event) { testFuelConsumption = event.New_Fuel_Consumption; ++fuelConsumptionEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<AirSpeedChanged>([&](const auto& event) { testAirSpeed = event.New_Air_Speed; ++airSpeedEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<FuelRangeChanged>([&](const auto& event) { testFuelRange = event.New_Fuel_Range; ++fuelRangeEventCount; fuelService.SetFuelRangeFlag(false); });
    std::ostringstream oss; // Set up oss to capture output stream
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    std::thread sensorThread(invalidLoop, std::ref(bus)); 
    sensorThread.join();
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_EQ(fuelLevelEventCount, 0);
    EXPECT_EQ(fuelConsumptionEventCount, 0);
    EXPECT_EQ(airSpeedEventCount, 0);
    EXPECT_EQ(fuelRangeEventCount, 0);
    EXPECT_EQ(testFuelLevel, 0.0f); 
    EXPECT_EQ(testFuelConsumption, 0.0f);
    EXPECT_EQ(testAirSpeed, 0.0f);
    EXPECT_EQ(testFuelRange, 0.0f); 
}

TEST(EndToEndIntegrationTest, SensorConsensusRecoveryRestoresNormalOperation) {
    EventBus bus;
    FuelService fuelService(bus, TestConfig());
    FuelDisplayService fuelDisplayService(bus);
    SensorProcessingService sensorProcessingService(bus, TestConfig());

    int fuelLevelEventCount = 0, 
        fuelConsumptionEventCount = 0, 
        airSpeedEventCount = 0, 
        fuelRangeEventCount = 0,
        lowFuelRaisedEventCount = 0,
        criticalFuelEventCount = 0;
    float testFuelLevel = 0.0f, 
          testFuelConsumption = 0.0f, 
          testAirSpeed = 0.0f, 
          testFuelRange = 0.0f; 
    bus.subscribe<FuelLevelChanged>([&](const auto& event) { testFuelLevel = event.New_Fuel_Level; ++fuelLevelEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<FuelConsumptionChanged>([&](const auto& event) { testFuelConsumption = event.New_Fuel_Consumption; ++fuelConsumptionEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<AirSpeedChanged>([&](const auto& event) { testAirSpeed = event.New_Air_Speed; ++airSpeedEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<FuelRangeChanged>([&](const auto& event) { testFuelRange = event.New_Fuel_Range; ++fuelRangeEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<LowFuelWarningRaised>([&](const auto& event) { ++lowFuelRaisedEventCount; fuelService.SetFuelRangeFlag(false); });
    bus.subscribe<CriticalFuelWarningRaised>([&](const auto& event) { ++criticalFuelEventCount; fuelService.SetFuelRangeFlag(false); });
    std::ostringstream oss; // Set up oss to capture output stream
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    std::thread sensorThread(recoveryLoop, std::ref(bus)); 
    sensorThread.join();
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    float calculatedFuelRange = (198.5f/0.0321f)*251.5f; 
    EXPECT_EQ(fuelLevelEventCount, 8);
    EXPECT_EQ(fuelConsumptionEventCount, 8);
    EXPECT_EQ(airSpeedEventCount, 4);
    EXPECT_EQ(fuelRangeEventCount, 20);
    EXPECT_EQ(testFuelLevel, 198.5f); 
    EXPECT_EQ(testFuelConsumption, 0.0321f);
    EXPECT_EQ(testAirSpeed, 251.5f);
    EXPECT_NEAR(testFuelRange, calculatedFuelRange, 1.0f); 
}