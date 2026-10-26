#include <gtest/gtest.h>

#include "event-bus/event-bus.h"
#include "event-bus/events.h"
#include "fuel/fuel-service.h"
#include "sitl-sensors/fuel-sensor.h"
#include "sitl-sensors/air-speed-sensor.h"

namespace {

AircraftConfiguration TestConfig() {
    AircraftConfiguration config{};

    config.fuelMetrics.Initial_Fuel_Level = 100.0f;
    config.fuelThresholds.low_fuel_warning_threshold = 20.0f;
    config.fuelThresholds.critical_fuel_warning_threshold = 10.0f;

    return config;
}

} // namespace

TEST(SensorToFuelServiceIntegrationTests, ChangeInFuelSensorOutputChangesFuelLevel) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    FuelSensor sensor(bus, TestConfig());
    int counts = 0;
    bus.subscribe<NewFuelLevelSensorOutput>([&](const auto& event) { ++counts; });
    sensor.GetData();
    EXPECT_EQ(counts, 1);
}

TEST(SensorToFuelServiceIntegrationTests, NoFuelSensorEventBeforeGetData) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    FuelSensor sensor(bus, TestConfig());
    int counts = 0;
    bus.subscribe<NewFuelLevelSensorOutput>([&](const auto& event) { ++counts; });
    EXPECT_EQ(counts, 0);
}

TEST(SensorToFuelServiceIntegrationTests, MultipleFuelSensorReadingsPublishMultipleEvents) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    FuelSensor sensor(bus, TestConfig());
    int counts = 0;
    bus.subscribe<NewFuelLevelSensorOutput>([&](const auto& event) { ++counts; });
    sensor.GetData();
    EXPECT_EQ(counts, 1);
    sensor.GetData();
    EXPECT_EQ(counts, 2);
}

TEST(SensorToFuelServiceIntegrationTests, MultipleSubscribersReceiveFuelSensorEvent) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    FuelSensor sensor(bus, TestConfig());
    int firstSubscriberCount = 0;
    int secondSubscriberCount = 0;
    bus.subscribe<NewFuelLevelSensorOutput>([&](const auto& event) { ++firstSubscriberCount; });
    bus.subscribe<NewFuelLevelSensorOutput>([&](const auto& event) { ++secondSubscriberCount; });
    sensor.GetData();
    EXPECT_EQ(firstSubscriberCount, 1);
    EXPECT_EQ(secondSubscriberCount, 1);
}