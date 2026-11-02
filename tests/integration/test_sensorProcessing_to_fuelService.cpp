#include <gtest/gtest.h>

#include "event-bus/event-bus.h"
#include "event-bus/events.h"
#include "fuel/fuel-service.h"
#include "sensor-processing/sensor-processing-service.h"

namespace {

AircraftConfiguration TestConfig() {
    AircraftConfiguration config{};

    config.fuelMetrics.Initial_Fuel_Level = 100.0f;
    config.fuelThresholds.low_fuel_warning_threshold = 20.0f;
    config.fuelThresholds.critical_fuel_warning_threshold = 10.0f;

    return config;
}

} // namespace

TEST(SensorProcessingServiceToFuelServiceIntegrationTests, ValidatedFuelLevelIsRecievedByFuelService) {
    EventBus bus;
    FuelService service(bus, TestConfig());

    // Preset fuel level
    service.CheckFuelLevelChanged(150.0f);
    int counts = 0;
    float testFuelLevel; 
    bus.subscribe<FuelLevelChanged>([&](const auto& event) { ++counts; testFuelLevel = event.New_Fuel_Level;});
    float inputFuelLevel = 100.0f;
    bus.publish<ValidatedFuelLevelOutput>({inputFuelLevel});
    EXPECT_EQ(testFuelLevel, inputFuelLevel);
    EXPECT_EQ(counts, 1);
}

TEST(SensorProcessingServiceToFuelServiceIntegrationTests, ValidatedFuelConsumptionIsRecievedByFuelService) {
    EventBus bus;
    FuelService service(bus, TestConfig());

    // Preset fuel consumption
    service.CheckFuelConsumptionChanged(150.0f);
    int counts = 0;
    float testFuelConsumption; 
    bus.subscribe<FuelConsumptionChanged>([&](const auto& event) { ++counts; testFuelConsumption = event.New_Fuel_Consumption;});
    float inputFuelConsumption = 0.02f;
    bus.publish<ValidatedFuelConsumptionOutput>({inputFuelConsumption});
    EXPECT_EQ(testFuelConsumption, inputFuelConsumption);
    EXPECT_EQ(counts, 1);
}

TEST(SensorProcessingServiceToFuelServiceIntegrationTests, ValidatedAirSpeedIsRecievedByFuelService) {
    EventBus bus;
    FuelService service(bus, TestConfig());

    // Preset fuel consumption
    service.CheckAirSpeedChanged(200.0f);
    int counts = 0;
    float testAirSpeed; 
    bus.subscribe<AirSpeedChanged>([&](const auto& event) { ++counts; testAirSpeed = event.New_Air_Speed;});
    float inputAirSpeed = 150.0f;
    bus.publish<ValidatedAirSpeedOutput>({inputAirSpeed});
    EXPECT_EQ(testAirSpeed, inputAirSpeed);
    EXPECT_EQ(counts, 1);
}

TEST(SensorProcessingServiceToFuelServiceIntegrationTests, MultipleValidatedSensorUpdatesAreAllRecievedByFuelService) {
    EventBus bus;
    FuelService service(bus, TestConfig());

    // Preset values
    service.CheckFuelLevelChanged(150.0f);
    service.CheckFuelConsumptionChanged(150.0f);
    service.CheckAirSpeedChanged(200.0f);

    // setup subscriptions
    int fuelLevelCounts = 0, fuelConsumptionCounts = 0, airSpeedCounts = 0, fuelRangeCounts = 0;
    float testFuelLevel, testFuelConsumption, testAirSpeed, testFuelRange; 
    bus.subscribe<FuelLevelChanged>([&](const auto& event) { ++fuelLevelCounts; testFuelLevel = event.New_Fuel_Level; });
    bus.subscribe<FuelConsumptionChanged>([&](const auto& event) { ++fuelConsumptionCounts; testFuelConsumption = event.New_Fuel_Consumption; });
    bus.subscribe<AirSpeedChanged>([&](const auto& event) { ++airSpeedCounts; testAirSpeed = event.New_Air_Speed;});
    bus.subscribe<FuelRangeChanged>([&](const auto& event) { ++fuelRangeCounts; testFuelRange = event.New_Fuel_Range; });
    // Define inputs
    float inputFuelLevel =  100.0f, inputFuelConsumption = 0.02f, inputAirSpeed = 150.0f;
    float inputFuelRange = (inputFuelLevel/inputFuelConsumption)*inputAirSpeed;
    // Run events
    bus.publish<ValidatedFuelLevelOutput>({inputFuelLevel});
    service.SetFuelRangeFlag(false);
    bus.publish<ValidatedFuelConsumptionOutput>({inputFuelConsumption});
    service.SetFuelRangeFlag(false);
    bus.publish<ValidatedAirSpeedOutput>({inputAirSpeed});
    service.SetFuelRangeFlag(false);
    EXPECT_EQ(inputFuelLevel, testFuelLevel);
    EXPECT_EQ(inputFuelConsumption, testFuelConsumption);
    EXPECT_EQ(inputAirSpeed, testAirSpeed);
    EXPECT_EQ(inputFuelRange, testFuelRange);
}