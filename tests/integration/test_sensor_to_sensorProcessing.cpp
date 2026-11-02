#include <gtest/gtest.h>
#include <sitl-sensors/air-speed-sensor.h>
#include <sitl-sensors/fuel-sensor.h>
#include <event-bus/event-bus.h>
#include <event-bus/events.h>
#include <sensor-processing/sensor-processing-service.h>

namespace {
    AircraftConfiguration TestConfig() {
        AircraftConfiguration config{};

        config.fuelMetrics.Initial_Fuel_Level = 100.0f;
        config.fuelThresholds.low_fuel_warning_threshold = 20.0f;
        config.fuelThresholds.critical_fuel_warning_threshold = 10.0f;

        return config;
    }
} // namespace


TEST(SensorToSensorProcessingServiceIntegrationTest, AirSensorOutputIsValidatedBySensorProcessingService) {
    EventBus bus;
    AirSpeedSensor sensor(bus, TestConfig());
    SensorProcessingService service(bus, TestConfig());

    int eventCount = 0;
    float testAirSpeed = 0.0f;
    bus.subscribe<ValidatedAirSpeedOutput>([&](const auto& event) {++eventCount; testAirSpeed = event.Valid_Air_Speed; });

    // Trigger air speed data output
    sensor.GetData();

    // Check that outputted value is within expected limits
    bool limitFlag = false;
    if (testAirSpeed >= TestConfig().airspeeds.minimum_airspeed && testAirSpeed <= TestConfig().airspeeds.maximum_airspeed) {
        limitFlag = true;
    }
    EXPECT_EQ(eventCount, 1);
    EXPECT_TRUE(limitFlag);
}

TEST(SensorToSensorProcessingServiceIntegrationTest, FuelSensorOutputIsValidatedBySensorProcessingService) {
    EventBus bus;
    FuelSensor sensor(bus, TestConfig());
    SensorProcessingService service(bus, TestConfig());

    int fuelLevelEventCount = 0, fuelConsumptionEventCount = 0;
    float testFuelLevel = 0.0f, testFuelConsumption = 0.0f;
    bus.subscribe<ValidatedFuelLevelOutput>([&](const auto& event) {++fuelLevelEventCount; testFuelLevel = event.Valid_Fuel_Level; });
    bus.subscribe<ValidatedFuelConsumptionOutput>([&](const auto& event) {++fuelConsumptionEventCount; testFuelConsumption = event.Valid_Fuel_Consumption; });

    // Trigger fuel level data output
    sensor.GetData();

    // Verify that sensor readings fall within expected ranges
    bool FuelLevelLimitFlag = false, FuelConsumptionLimitFlag = false;
    if (testFuelLevel >= 0.0f && testFuelLevel <= TestConfig().fuelMetrics.fuel_capacity) {
        FuelLevelLimitFlag = true;
    }
    if (testFuelConsumption >= TestConfig().fuelMetrics.fuel_consumption_minimum && testFuelConsumption <= TestConfig().fuelMetrics.fuel_consumption_maximum) {
        FuelConsumptionLimitFlag = true;
    }
    EXPECT_TRUE(FuelLevelLimitFlag);    
    EXPECT_TRUE(FuelConsumptionLimitFlag);
    EXPECT_EQ(fuelLevelEventCount, 3);
    EXPECT_NE(testFuelLevel, 0.0f);
    EXPECT_EQ(fuelConsumptionEventCount, 3); 
    EXPECT_NE(testFuelConsumption, 0.0f);
}
