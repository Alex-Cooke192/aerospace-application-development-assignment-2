#include <string>
#include <gtest/gtest.h>
#include <sstream>
#include "../../src/event-bus/event-bus.h"
#include "../../src/fuel/fuel-service.h"
#include "../../src/fuel-display/fuel-display-service.h"
#include "../../.config/aircraft-configuration.h"
#include "../../src/event-bus/events.h"

namespace {
AircraftConfiguration TestConfig() {
    AircraftConfiguration config;
    config.fuelMetrics.Initial_Fuel_Level = 100.0f;
    config.fuelThresholds.low_fuel_warning_threshold = 20.0f;
    config.fuelThresholds.critical_fuel_warning_threshold = 10.0f;
    return config;
}
}

TEST(FuelSerivceToDisplayIntegrationTest, FuelLevelChangePrintsNewFuelLevel) {
    EventBus bus;
    FuelService service(bus, TestConfig());
    FuelDisplayService display(bus);
    int eventCount = 0;
    float testFuelLevel = 0.0; 
    bus.subscribe<FuelLevelChanged>([&](const auto& event) { testFuelLevel = event.New_Fuel_Level; ++eventCount; } );
    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    service.CheckFuelLevelChanged(40.0f);
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_EQ(testFuelLevel, 40.0); // Verifies calculation
    EXPECT_NE(oss.str().find("40"), std::string::npos); // Verifies terminal display output
}

TEST(FuelSerivceToDisplayIntegrationTest, FuelConsumptionChangePrintsNewFuelRange) {
    EventBus bus;
    FuelDisplayService display(bus);
    FuelService service(bus, TestConfig());
    service.CheckAirSpeedChanged(150.0f);
    service.CheckFuelLevelChanged(100.0f);
    service.CheckFuelConsumptionChanged(0.03f);
    int eventCount = 0;
    float testFuelRange = 0.0;
    bus.subscribe<FuelRangeChanged>([&](const auto& event) { testFuelRange = event.New_Fuel_Range; ++eventCount; });
    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf());
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    service.SetFuelRangeFlag(false);
    service.CheckFuelConsumptionChanged(0.01f);
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("RANGE"), std::string::npos); // Verifies terminal output
    EXPECT_EQ(eventCount, 1);
    EXPECT_EQ(testFuelRange, 1500000.0); // Verifies calculation
}

TEST(FuelServiceToDisplayIntegrationTest, AirSpeedChangedPrintsNewFuelRange) {
    EventBus bus;
    FuelDisplayService display(bus);
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(0.01f);
    service.CheckFuelLevelChanged(100.0f);
    service.CheckAirSpeedChanged(200.0f);
    int eventCount = 0;
    float testFuelRange = 0.0; // Must be declared here so can be seen by EXPECT_EQ lines
    bus.subscribe<FuelRangeChanged>([&](const auto& event) { testFuelRange = event.New_Fuel_Range; ++eventCount; });
    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf());
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead 
    service.SetFuelRangeFlag(false); 
    service.CheckAirSpeedChanged(150.0f);
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("RANGE"), std::string::npos); // erifies terminal output
    EXPECT_EQ(eventCount, 1);
    EXPECT_EQ(testFuelRange, 1500000.0); // Verifies calculation
}

TEST(FuelServiceToDisplayIntegrationTest, LowFuelLevelDisplaysLowFuelWarning) {
    EventBus bus;
    FuelDisplayService display(bus);
    FuelService service(bus, TestConfig());
    service.CheckFuelLevelChanged(100.0f);
    bus.subscribe<FuelLevelChanged>([&](const auto&) {} ); 
    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf());
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    service.CheckFuelLevelChanged(15.0f);
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("LOW"), std::string::npos);
}

TEST(FuelServiceToDisplayIntegrationTest, CriticalFuelLevelDisplaysCriticalFuelWarning) {
    EventBus bus;
    FuelDisplayService display(bus);
    FuelService service(bus, TestConfig());
    service.CheckFuelLevelChanged(100.0f);
    bus.subscribe<FuelLevelChanged>([&](const auto&) {} ); 
    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf());
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    service.CheckFuelLevelChanged(5.0f);
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("CRITICAL"), std::string::npos);
}

TEST(FuelServiceToDisplayIntegrationTest, LowFuelClearedDisplaysLowFuelCleared) {
    EventBus bus;
    FuelDisplayService display(bus);
    FuelService service(bus, TestConfig());
    service.CheckFuelLevelChanged(15.0f);
    bus.subscribe<FuelLevelChanged>([&](const auto&) {} ); 
    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf());
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    service.CheckFuelLevelChanged(100.0f);
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("CLEARED"), std::string::npos);
    EXPECT_NE(oss.str().find("NORMAL"), std::string::npos);
}

TEST(FuelServiceToDisplayIntegrationTest, CriticalFuelClearedDisplaysCriticalFuelClearedAndRaisesLowFuelWarning) {
    EventBus bus;
    FuelDisplayService display(bus);
    FuelService service(bus, TestConfig());
    service.CheckFuelLevelChanged(5.0f);
    bus.subscribe<FuelLevelChanged>([&](const auto&) {} ); 
    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf());
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    service.CheckFuelLevelChanged(15.0f);
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("CLEARED"), std::string::npos);
    EXPECT_NE(oss.str().find("CRITICAL"), std::string::npos);
    EXPECT_NE(oss.str().find("LOW"), std::string::npos); 
}
