#include <gtest/gtest.h>
#include "event-bus/event-bus.h"
#include "sensor-processing/sensor-processing-service.h"
#include "aircraft-configuration.h"

namespace {
AircraftConfiguration TestConfig() {
    return AircraftConfiguration{}; 
}
}

// ------------------------------------------------------------------------------------------------------
// Fuel level data tests

TEST(sensorProcessingService, ValidFuelLevelDataIsAccepted) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testFuelLevel = 0.0f;
    bus.subscribe<ValidatedFuelLevelOutput>(
        [&](const ValidatedFuelLevelOutput& event) { ++eventCount; testFuelLevel = event.Valid_Fuel_Level; });
    bus.publish<NewFuelLevelSensorOutput>({
    200.0f, 201.2f, 199.5f, 200.8f, 198.9f});
    EXPECT_EQ(eventCount, 3);
    EXPECT_EQ(testFuelLevel, 200.8f);
}

TEST(sensorProcessingService, OnlyThreeFuelLevelReadingsAgree_IgnoresInvalidReadings) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testFuelLevel = 0.0f;
    bus.subscribe<ValidatedFuelLevelOutput>(
        [&](const ValidatedFuelLevelOutput& event) { ++eventCount; testFuelLevel = event.Valid_Fuel_Level; });
    
    bus.publish<NewFuelLevelSensorOutput>({
    200.0f, 201.0f, 199.0f, 150.0f, 245.0f});
    EXPECT_EQ(eventCount, 1);
    EXPECT_EQ(testFuelLevel, 200.0f);
}

TEST(sensorProcessingService, OnlyTwoFuelLevelReadingsAgree_DoesNotPublishEvent) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testFuelLevel = 0.0f;
    bus.subscribe<ValidatedFuelLevelOutput>(
        [&](const ValidatedFuelLevelOutput& event) { ++eventCount; testFuelLevel = event.Valid_Fuel_Level; });
    bus.publish<NewFuelLevelSensorOutput>({ 200.0f , 202.0f , 50.0f , 100.0f , 150.0f });
    EXPECT_EQ(eventCount, 0);
    EXPECT_EQ(testFuelLevel, 0.0f);
}

TEST(sensorProcessingService, NoFuelLevelReadingsAgree_DoesNotPublishEvent) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testFuelLevel = 0.0f;
    bus.subscribe<ValidatedFuelLevelOutput>(
        [&](const ValidatedFuelLevelOutput& event) { ++eventCount; testFuelLevel = event.Valid_Fuel_Level; });
    bus.publish<NewFuelLevelSensorOutput>({ 50.0f , 80.0f , 110.0f , 140.0f , 170.0f });
    EXPECT_EQ(eventCount, 0);
    EXPECT_EQ(testFuelLevel, 0.0f);
}

TEST(sensorProcessingService, InvalidFuelDataRobustnessTesting) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testFuelLevel = 0.0f;
    bus.subscribe<ValidatedFuelLevelOutput>(
        [&](const ValidatedFuelLevelOutput& event) { ++eventCount; testFuelLevel = event.Valid_Fuel_Level; });
    bus.publish<NewFuelLevelSensorOutput>({ INFINITY , 80.0f , 110.0f , 140.0f , 170.0f });
    bus.publish<NewFuelLevelSensorOutput>({ 50.0f , -80.0f , 110.0f , 140.0f , 170.0f });
    bus.publish<NewFuelLevelSensorOutput>({ 50.0f , 80.0f , 11110.0f , 140.0f , 170.0f });
    bus.publish<NewFuelLevelSensorOutput>({ INFINITY , -80.0f , 11110.0f , 140.0f , 170.0f });
    EXPECT_EQ(eventCount, 0);
    EXPECT_EQ(testFuelLevel, 0.0f);
}

// ------------------------------------------------------------------------------------------------------
// Fuel consumption data tests

TEST(sensorProcessingService, ValidFuelConsumptionDataIsAccepted) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testFuelConsumption = 0.0f;
    bus.subscribe<ValidatedFuelConsumptionOutput>(
        [&](const ValidatedFuelConsumptionOutput& event) { ++eventCount; testFuelConsumption = event.Valid_Fuel_Consumption; });
    bus.publish<NewFuelConsumptionSensorOutput>({0.0300f, 0.0302f, 0.0299f, 0.0301f, 0.0298f});
    EXPECT_EQ(eventCount, 3);
    EXPECT_EQ(testFuelConsumption, 0.0301f); 
}

TEST(sensorProcessingService, OnlyThreeFuelConsumptionReadingsAgree_IgnoresInvalidReadings) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testFuelConsumption = 0.0f;
    bus.subscribe<ValidatedFuelConsumptionOutput>(
        [&](const ValidatedFuelConsumptionOutput& event) { ++eventCount; testFuelConsumption = event.Valid_Fuel_Consumption; });
    bus.publish<NewFuelConsumptionSensorOutput>({ 0.0300f , 0.0302f , 0.0299f , 0.0120f , 0.0490f });
    EXPECT_EQ(eventCount, 1);
    EXPECT_EQ(testFuelConsumption, 0.0300f); 
}

TEST(sensorProcessingService, OnlyTwoFuelConsumptionReadingsAgree_DoesNotPublishEvent) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testFuelConsumption = 0.0f;
    bus.subscribe<ValidatedFuelConsumptionOutput>(
        [&](const ValidatedFuelConsumptionOutput& event) { ++eventCount; testFuelConsumption = event.Valid_Fuel_Consumption; });
    bus.publish<NewFuelConsumptionSensorOutput>({ 0.0300f , 0.0305f , 0.0100f , 0.0200f , 0.0450f });
    EXPECT_EQ(eventCount, 0);
    EXPECT_EQ(testFuelConsumption, 0.0f);
}

TEST(sensorProcessingService, NoFuelConsumptionReadingsAgree_DoesNotPublishEvent) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testFuelConsumption = 0.0f;
    bus.subscribe<ValidatedFuelConsumptionOutput>(
        [&](const ValidatedFuelConsumptionOutput& event) { ++eventCount; testFuelConsumption = event.Valid_Fuel_Consumption; });
    bus.publish<NewFuelConsumptionSensorOutput>({ 0.010f , 0.020f , 0.030f , 0.040f , 0.050f });
    EXPECT_EQ(eventCount, 0);
    EXPECT_EQ(testFuelConsumption, 0.0f); 
}

TEST(sensorProcessingService, InvalidFuelConsumptionDataRobustnessTesting) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0;
    float testFuelConsumption = 0.0f;
    bus.subscribe<ValidatedFuelConsumptionOutput>(
        [&](const ValidatedFuelConsumptionOutput& event) { ++eventCount; testFuelConsumption = event.Valid_Fuel_Consumption;});
    bus.publish<NewFuelConsumptionSensorOutput>({ INFINITY, 0.020f, 0.030f, 0.040f, 0.050f });
    bus.publish<NewFuelConsumptionSensorOutput>({ 0.010f, -0.020f, 0.030f, 0.040f, 0.050f });
    bus.publish<NewFuelConsumptionSensorOutput>({ 0.010f, 0.020f, 1.000f, 0.040f, 0.050f });
    bus.publish<NewFuelConsumptionSensorOutput>({ INFINITY, -0.020f, 1.000f, 0.040f, 0.050f });
    EXPECT_EQ(eventCount, 0);
    EXPECT_EQ(testFuelConsumption, 0.0f);
}
// ------------------------------------------------------------------------------------------------------
// Air speed data tests

TEST(sensorProcessingService, ValidAirSpeedDataIsAccepted) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testAirSpeed = 0.0f;
    bus.subscribe<ValidatedAirSpeedOutput>(
        [&](const ValidatedAirSpeedOutput& event) { ++eventCount; testAirSpeed = event.Valid_Air_Speed; });
    
    bus.publish<NewAirSpeedSensorOutput>({250.0f, 251.2f, 249.5f, 250.8f, 248.9f});
    EXPECT_EQ(eventCount, 1);
    EXPECT_EQ(testAirSpeed, 249.5f); 
}

TEST(sensorProcessingService, OnlyThreeAirSpeedReadingsAgree_IgnoresInvalidReadings) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testAirSpeed = 0.0f;
    bus.subscribe<ValidatedAirSpeedOutput>(
        [&](const ValidatedAirSpeedOutput& event) { ++eventCount; testAirSpeed = event.Valid_Air_Speed; });
    
    bus.publish<NewAirSpeedSensorOutput>({ 248.0f , 249.5f , 252.0f , 100.0f , 350.0f });
    EXPECT_EQ(eventCount, 1);
    EXPECT_EQ(testAirSpeed, 249.5f); 
}

TEST(sensorProcessingService, OnlyTwoAirSpeedReadingsAgree_DoesNotPublishEvent) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testAirSpeed = 0.0f;
    bus.subscribe<ValidatedAirSpeedOutput>(
        [&](const ValidatedAirSpeedOutput& event) { ++eventCount; testAirSpeed = event.Valid_Air_Speed; });
    
    bus.publish<NewAirSpeedSensorOutput>({ 250.0f, 252.0f , 100.0f , 150.0f , 350.0f });
    EXPECT_EQ(eventCount, 0);
    EXPECT_EQ(testAirSpeed, 0.0f); 
}

TEST(sensorProcessingService, NoAirSpeedReadingsAgree_DoesNotPublishEvent) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0; 
    float testAirSpeed = 0.0f;
    bus.subscribe<ValidatedAirSpeedOutput>(
        [&](const ValidatedAirSpeedOutput& event) { ++eventCount; testAirSpeed = event.Valid_Air_Speed; });
    bus.publish<NewAirSpeedSensorOutput>({ 100.0f , 150.0f , 200.0f , 250.0f , 300.0f });
    EXPECT_EQ(eventCount, 0);
    EXPECT_EQ(testAirSpeed, 0.0f); 
}

TEST(sensorProcessingService, InvalidAirSpeedDataRobustnessTesting) {
    EventBus bus;
    SensorProcessingService service(bus, TestConfig());
    int eventCount = 0;
    float testAirSpeed = 0.0f;
    bus.subscribe<ValidatedAirSpeedOutput>(
        [&](const ValidatedAirSpeedOutput& event) { ++eventCount; testAirSpeed = event.Valid_Air_Speed; }); 
    bus.publish<NewAirSpeedSensorOutput>({ INFINITY, 100.0f, 150.0f, 200.0f, 250.0f });
    bus.publish<NewAirSpeedSensorOutput>({ 50.0f, -100.0f, 150.0f, 200.0f, 250.0f });
    bus.publish<NewAirSpeedSensorOutput>({ 50.0f, 100.0f, 1000.0f, 200.0f, 250.0f });
    bus.publish<NewAirSpeedSensorOutput>({ INFINITY, -100.0f, 1000.0f, 200.0f, 250.0f });
    EXPECT_EQ(eventCount, 0);
    EXPECT_EQ(testAirSpeed, 0.0f);
}