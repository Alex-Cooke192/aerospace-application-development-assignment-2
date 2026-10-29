#include <gtest/gtest.h>
#include "fuel-display/fuel-display-service.h"
#include "event-bus/events.h"
#include <string>

TEST(FuelDisplayServiceTest, PrintsFuelLevelChange) {
    EventBus bus;
    FuelDisplayService display(bus);
    testing::internal::CaptureStdout();
    display.PrintFuelLevelChanged(80.0f, 50.0f);
    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("FUEL LEVEL CHANGED"), std::string::npos);
    EXPECT_NE(output.find("Original: 80"), std::string::npos);
    EXPECT_NE(output.find("New:50"), std::string::npos);
}

TEST(FuelDisplayServiceTest, PrintsLowFuelWarning) {
    EventBus bus;
    FuelDisplayService display(bus);
    testing::internal::CaptureStdout();
    display.PrintLowFuelWarning(15.0f);
    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("WARNING: FUEL LOW"), std::string::npos);
    EXPECT_NE(output.find("Fuel level: 15"), std::string::npos);
}

TEST(FuelDisplayServiceTest, PrintsCriticalFuelWarning) {
    EventBus bus;
    FuelDisplayService display(bus);
    testing::internal::CaptureStdout();
    display.PrintCriticalFuelWarning(5.0f);
    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("WARNING: FUEL CRITICAL"), std::string::npos);
    EXPECT_NE(output.find("Fuel level: 5"), std::string::npos);
}

TEST(FuelDisplayServiceTest, PrintsLowWarningCleared) {
    EventBus bus;
    FuelDisplayService display(bus);
    testing::internal::CaptureStdout();
    display.PrintLowFuelClear(30.0f);
    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("WARNING CLEARED: FUEL NORMAL"), std::string::npos);
}

TEST(FuelDisplayServiceTest, PrintsCriticalWarningCleared) {
    EventBus bus;
    FuelDisplayService display(bus);
    testing::internal::CaptureStdout();
    display.PrintCriticalFuelWarningCleared(15.0f);
    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("CRITICAL WARNING CLEARED: FUEL LOW"), std::string::npos);
}

TEST(FuelDisplayServiceTest, PrintsConsumptionChange) {
    EventBus bus;
    FuelDisplayService display(bus);
    testing::internal::CaptureStdout();
    display.PrintFuelConsumptionChanged(2.0f, 3.0f);
    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("FUEL CONSUMPTION CHANGED"), std::string::npos);
    EXPECT_NE(output.find("New:3"), std::string::npos);
}

TEST(FuelDisplayServiceTest, PrintsRangeChange) {
    EventBus bus;
    FuelDisplayService display(bus);
    testing::internal::CaptureStdout();
    display.PrintRangeChanged(100.0f, 50.0f);
    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("FUEL RANGE CHANGED"), std::string::npos);
    EXPECT_NE(output.find("New:50"), std::string::npos);
}
