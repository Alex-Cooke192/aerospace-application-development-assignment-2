#include <gtest/gtest.h>
#include "fuel/fuel-service.h"
#include "event-bus/events.h"
#include "aircraft-configuration.h"

namespace {
AircraftConfiguration TestConfig() {
    AircraftConfiguration config;
    config.fuelMetrics.Initial_Fuel_Level = 100.0f;
    config.fuelThresholds.low_fuel_warning_threshold = 20.0f;
    config.fuelThresholds.critical_fuel_warning_threshold = 10.0f;
    return config;
}
}

TEST(FuelServiceTest, AirSpeedChangePublishesOldAndNewValues) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    int calls = 0;
    float oldValue = -1, newValue = -1;
    bus.subscribe<AirSpeedChanged>([&](const AirSpeedChanged& e) {
        ++calls;
        oldValue = e.Original_Air_Speed;
        newValue = e.New_Air_Speed;
    });
    service.CheckAirSpeedChanged(150.0f);
    EXPECT_EQ(calls, 1);
    EXPECT_FLOAT_EQ(oldValue, 0.0f);
    EXPECT_FLOAT_EQ(newValue, 150.0f);
}

TEST(FuelServiceTest, UnchangedAirSpeedDoesNotRepublish) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    int calls = 0;
    bus.subscribe<AirSpeedChanged>([&](const auto&) { ++calls; });
    service.CheckAirSpeedChanged(150.0f);
    service.CheckAirSpeedChanged(150.0f);
    EXPECT_EQ(calls, 1);
}

TEST(FuelServiceTest, FuelLevelChangePublishesOriginalAndNewLevels) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    // Initialise consumption first: the range calculation divides by this value.
    service.CheckFuelConsumptionChanged(2.0f);
    int calls = 0;
    float before = -1, after = -1;
    bus.subscribe<FuelLevelChanged>([&](const FuelLevelChanged& e) {
        ++calls;
        before = e.Original_Fuel_Level;
        after = e.New_Fuel_Level;
    });
    service.CheckFuelLevelChanged(75.0f);
    EXPECT_EQ(calls, 1);
    EXPECT_FLOAT_EQ(before, 100.0f);
    EXPECT_FLOAT_EQ(after, 75.0f);
}

TEST(FuelServiceTest, RaisesLowWarningBelowThreshold) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    int warnings = 0;
    bus.subscribe<LowFuelWarningRaised>([&](const auto&) { ++warnings; });
    service.CheckFuelLevelChanged(15.0f);
    EXPECT_EQ(warnings, 1);
}

TEST(FuelServiceTest, DoesNotRaiseLowWarningAtThreshold) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    int warnings = 0;
    bus.subscribe<LowFuelWarningRaised>([&](const auto&) { ++warnings; });
    service.CheckFuelLevelChanged(20.0f);
    EXPECT_EQ(warnings, 0); // Current production comparison is strictly '<'.
}

TEST(FuelServiceTest, RaisesCriticalWarningBelowCriticalThreshold) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    int warnings = 0;
    bus.subscribe<CriticalFuelWarningRaised>([&](const auto&) { ++warnings; });
    service.CheckFuelLevelChanged(5.0f);
    EXPECT_EQ(warnings, 1);
}

TEST(FuelServiceTest, DoesNotRaiseCriticalWarningAtThreshold) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    int warnings = 0;
    bus.subscribe<CriticalFuelWarningRaised>([&](const auto&) { ++warnings; });
    service.CheckFuelLevelChanged(10.0f);
    EXPECT_EQ(warnings, 0);
}

TEST(FuelServiceTest, ClearsLowWarningWhenFuelRecovers) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    int cleared = 0;
    bus.subscribe<LowFuelWarningCleared>([&](const auto&) { ++cleared; });
    service.CheckFuelLevelChanged(15.0f);
    service.CheckFuelLevelChanged(30.0f);
    EXPECT_EQ(cleared, 1);
}

TEST(FuelServiceTest, ClearsCriticalWarningWhenFuelRecovers) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    int cleared = 0;
    bus.subscribe<CriticalFuelWarningCleared>([&](const auto&) { ++cleared; });
    service.CheckFuelLevelChanged(5.0f);
    service.CheckFuelLevelChanged(15.0f);
    EXPECT_EQ(cleared, 1);
}
