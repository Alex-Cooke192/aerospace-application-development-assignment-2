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

TEST(FuelServiceTest, GetterAndSetterMethodsFunctionCorrectly) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    bool flag = service.GetFuelRangeFlag();
    service.SetFuelRangeFlag(true);
    bool secondFlag = service.GetFuelRangeFlag();
    EXPECT_FALSE(flag);
    EXPECT_TRUE(secondFlag);
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

TEST(FuelServiceTest, UnchangedFuelLevelDoesNotRepublish) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    int calls = 0;
    bus.subscribe<FuelLevelChanged>([&](const auto&) { ++calls; });
    service.CheckFuelLevelChanged(150.0f);
    service.CheckFuelLevelChanged(150.0f);
    EXPECT_EQ(calls, 1);
}

TEST(FuelServiceTest, FuelConsumptionChangedPublishedOriginalAndNewValues) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    int calls = 0;
    bus.subscribe<FuelConsumptionChanged>([&](const auto&) { ++calls; });
    service.CheckFuelConsumptionChanged(0.03f);
    service.CheckFuelConsumptionChanged(0.03f);
    EXPECT_EQ(calls, 1);
}

TEST(FuelServiceTest, FuelRangeFlagTrueDoesNotTriggerFuelRangeCalculation) {
    EventBus bus; 
    FuelService service(bus, TestConfig());
    int calls = 0;
    bus.subscribe<FuelRangeChanged>([&](const auto&) { ++calls; });
    bool flag = service.GetFuelRangeFlag(); // false
    service.CheckFuelLevelChanged(150.0f); // This should trigger a publish of the event
    service.SetFuelRangeFlag(true); 
    service.CheckFuelLevelChanged(200.0f); // flag is TRUE so shouldn't trigger a publish
    service.SetFuelRangeFlag(false);
    EXPECT_EQ(calls, 1);
}

TEST(FuelServiceTest, RaisesLowWarningBelowThreshold) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    service.CheckFuelLevelChanged(50.0f);
    int warnings = 0;
    bus.subscribe<LowFuelWarningRaised>([&](const auto&) { ++warnings; });
    service.CheckFuelLevelChanged(15.0f);
    EXPECT_EQ(warnings, 1);
}

TEST(FuelServiceTest, RaisesLowWarningWhenTransitioningFromCriticalToLow) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    service.CheckFuelLevelChanged(5.0f);
    int warnings = 0;
    bus.subscribe<LowFuelWarningRaised>([&](const auto&) { ++warnings; });
    service.CheckLowFuelWarning();
    EXPECT_EQ(warnings, 1); 
}

TEST(FuelServiceTest, DoesNotRaiseLowWarningWhenInLowFuelState) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    service.CheckFuelLevelChanged(15.0f); 
    int warnings = 0;
    bus.subscribe<LowFuelWarningRaised>([&](const auto&) { ++warnings; });
    service.CheckLowFuelWarning();
    EXPECT_EQ(warnings, 0);
}

// EDGE CASE
TEST(FuelServiceTest, RaisesLowWarningAtThreshold) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    int warnings = 0;
    bus.subscribe<LowFuelWarningRaised>([&](const auto&) { ++warnings; });
    service.CheckFuelLevelChanged(TestConfig().fuelThresholds.low_fuel_warning_threshold);
    EXPECT_EQ(warnings, 1); // Current production comparison is strictly '<'.
}

TEST(FuelServiceTest, TransitionsFromLowWarningToCriticalWarningBelowCriticalFuelThreshold) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    int lowWarnings = 0;
    int criticalWarnings = 0;
    bus.subscribe<LowFuelWarningRaised>([&](const auto&) { ++lowWarnings; });
    bus.subscribe<CriticalFuelWarningRaised>([&](const auto&) { ++criticalWarnings; });
    service.CheckFuelLevelChanged(15.0f); // Raise low fuel warning

    EXPECT_EQ(lowWarnings, 1); 
    EXPECT_EQ(criticalWarnings, 0); 
    service.CheckFuelLevelChanged(5.0f); // Raise critical fuel warning
    EXPECT_EQ(lowWarnings, 1);
    EXPECT_EQ(criticalWarnings, 1);
}

TEST(FuelServiceTest, RaisesCriticalWarningBelowCriticalFuelThreshold) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    int warnings = 0;
    bus.subscribe<CriticalFuelWarningRaised>([&](const auto&) { ++warnings; });
    service.CheckFuelLevelChanged(50.0f);
    EXPECT_EQ(warnings, 0);
    service.CheckFuelLevelChanged(15.0f);
    EXPECT_EQ(warnings, 0);
    service.CheckFuelLevelChanged(5.0f);
    EXPECT_EQ(warnings, 1);
}

// EDGE CASE
TEST(FuelServiceTest, RaisesCriticalWarningAtThreshold) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    int warnings = 0;
    bus.subscribe<CriticalFuelWarningRaised>([&](const auto&) { ++warnings; });
    service.CheckFuelLevelChanged(TestConfig().fuelThresholds.critical_fuel_warning_threshold);
    EXPECT_EQ(warnings, 1);
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

TEST(FuelServiceTest, DoesNotClearLowWarningWhenStateIsNotWarn) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    int cleared = 0;
    bus.subscribe<LowFuelWarningCleared>([&](const auto&) { ++cleared; });
    service.CheckFuelLevelChanged(50.0f);
    service.CheckLowFuelWarningCleared(); 
    EXPECT_EQ(cleared, 0); 
}

TEST(FuelServiceTest, TransitionsFromCriticalWarningToLowWarningWhenFuelRecoversBelowLowThreshold) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    int lowCleared = 0;
    int criticalCleared = 0;
    bus.subscribe<CriticalFuelWarningCleared>([&](const auto&) { ++criticalCleared; });
    bus.subscribe<LowFuelWarningCleared>([&](const auto&) { ++lowCleared; }); 
    service.CheckFuelLevelChanged(5.0f);
    service.CheckFuelLevelChanged(15.0f);
    EXPECT_EQ(lowCleared, 0);
    EXPECT_EQ(criticalCleared, 1);
}

TEST(FuelServiceTest, DoesNotClearCriticalWarningWhenFuelDoesNotRecover) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(2.0f);
    int cleared = 0;
    bus.subscribe<CriticalFuelWarningCleared>([&](const auto&) { ++cleared; });
    service.CheckFuelLevelChanged(5.0f);
    service.CheckCriticalFuelWarningCleared();
    EXPECT_EQ(cleared, 0); 
}

TEST(FuelServiceTest, DoesNotClearCriticalWarningWhenStateIsNotCritical) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    service.CheckFuelLevelChanged(50.0f);
    int cleared = 0;
    bus.subscribe<CriticalFuelWarningCleared>([&](const auto&) { cleared++; });
    service.CheckFuelLevelChanged(50.0f);
    service.CheckCriticalFuelWarningCleared();
    EXPECT_EQ(cleared, 0);
}

TEST(FuelServiceTest, ChecksCriticalFuelWarningIsNotRaisedWhenInCriticalFuelState) {
    EventBus bus; 
    FuelService service(bus, TestConfig());
    service.CheckFuelLevelChanged(5.0);
    int warnings = 0;
    bus.subscribe<CriticalFuelWarningRaised>([&](const auto&) { warnings++; });
    service.CheckCriticalFuelWarning(); 
    EXPECT_EQ(warnings, 0);
}
