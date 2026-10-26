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
    bus.subscribe<FuelLevelChanged>([&](const auto&) {} );
    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    service.CheckFuelLevelChanged(40.0f);
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("40"), std::string::npos);
}

TEST(FuelSerivceToDisplayIntegrationTest, FuelConsumptionChangePrintsNewFuelRange) {
    EventBus bus;
    FuelDisplayService display(bus);
    FuelService service(bus, TestConfig());
    service.CheckAirSpeedChanged(150.0f);
    service.CheckFuelLevelChanged(100.0f);
    service.CheckFuelConsumptionChanged(0.03f);
    bus.subscribe<FuelConsumptionChanged>([&](const auto&) {} );
    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf());
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    service.SetFuelRangeFlag(false);
    service.CheckFuelConsumptionChanged(0.01f);
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("RANGE"), std::string::npos);
    // EXPECT_FLOAT_EQ(oss.str().find("1500000.0"), std::string::npos); // Checks correct calculation
}

TEST(FuelServiceToDisplayIntegrationTest, AirSpeedChangedPrintsNewFuelRange) {
    EventBus bus;
    FuelDisplayService display(bus);
    FuelService service(bus, TestConfig());
    service.CheckFuelConsumptionChanged(0.01f);
    service.CheckFuelLevelChanged(100.0f);
    service.CheckAirSpeedChanged(200.0f);
    bus.subscribe<AirSpeedChanged>([&](const auto&) {} );
    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf());
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead 
    service.SetFuelRangeFlag(false); 
    service.CheckAirSpeedChanged(150.0f);
    std::cout << "TEST" << std::endl; 
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("RANGE"), std::string::npos);
    // EXPECT_FLOAT_EQ(oss.str().find("1500000.0"), std::string::npos); // Checks correct calculation
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
