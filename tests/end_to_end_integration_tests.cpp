#include <gtest/gtest.h>
#include <sstream>
#include <unistd.h>
#include <thread>
#include <vector>

#include "event-bus/event-bus.h"
#include "event-bus/events.h"
#include "fuel/fuel-service.h"
#include "sitl-sensors/air-speed-sensor.h"
#include "sitl-sensors/fuel-sensor.h"
#include "fuel-display/fuel-display-service.h"

namespace {
    AircraftConfiguration TestConfig() {
        AircraftConfiguration config;
        
        config.fuelMetrics.Initial_Fuel_Level = 100.0f;
        config.fuelThresholds.low_fuel_warning_threshold = 20.0f;
        config.fuelThresholds.critical_fuel_warning_threshold = 10.0f;

        return config;
    };
    struct FuelLevelInputs { 
            float firstInput; 
            float secondInput; 
        } fuelLevelInputs{ 75.0f , 100.0f }, 
          lowFuelLevelInputs { 75.0f , 15.0f },
          criticalFuelLevelInputs { 15.0f , 5.0f };
    struct FuelConsumptionInputs { 
            float firstInput; 
            float secondInput; 
        } fuelConsumptionInputs{ 0.03 , 0.01 };
    struct FirstAirSpeedInput {
            float firstInput; 
            float secondInput; 
        } firstAirSpeedInput {0.0f, 250.0f};
    struct SecondAirSpeedInput {
            float firstInput; 
            float secondInput; 
        } secondAirSpeedInput {250.0f, 100.0f};

    struct AirSpeedInputs { 
        struct FirstAirSpeedInput firstAirSpeedInput; 
        struct SecondAirSpeedInput secondAirSpeedInput; 
    };

    struct LoopInputs {
        FuelLevelInputs fuelLevelInputs;
        FuelConsumptionInputs fuelConsumptionInputs;
        AirSpeedInputs airSpeedInputs;
    } loopInputs { fuelLevelInputs, fuelConsumptionInputs, {firstAirSpeedInput, secondAirSpeedInput}},
    lowLoopInputs { lowFuelLevelInputs, fuelConsumptionInputs,  {firstAirSpeedInput, secondAirSpeedInput}},
    criticalLoopInputs { criticalFuelLevelInputs, fuelConsumptionInputs, {firstAirSpeedInput, secondAirSpeedInput}};
    void criticalFuelLoop(EventBus& bus) {
        NewFuelLevelSensorOutput newFuelLevelOutput {criticalLoopInputs.fuelLevelInputs.firstInput};
        bus.publish(newFuelLevelOutput); 
        NewFuelConsumptionSensorOutput newFuelConsumptionOutput {criticalLoopInputs.fuelConsumptionInputs.firstInput};
        bus.publish(newFuelConsumptionOutput);
        NewAirSpeedSensorOutput newAirSpeedOutput {criticalLoopInputs.airSpeedInputs.firstAirSpeedInput.firstInput, loopInputs.airSpeedInputs.firstAirSpeedInput.secondInput};
        bus.publish(newAirSpeedOutput);

        newFuelLevelOutput.Output_Fuel_Level = criticalLoopInputs.fuelLevelInputs.secondInput;
        bus.publish(newFuelLevelOutput);
        newFuelConsumptionOutput.Output_Fuel_Consumption = criticalLoopInputs.fuelConsumptionInputs.secondInput;
        bus.publish(newFuelConsumptionOutput);
        newAirSpeedOutput.Output_Airspeed = criticalLoopInputs.airSpeedInputs.secondAirSpeedInput.secondInput;
        bus.publish(newAirSpeedOutput);
    }
    void lowFuelLoop(EventBus& bus) {
        NewFuelLevelSensorOutput newFuelLevelOutput {lowLoopInputs.fuelLevelInputs.firstInput};
        bus.publish(newFuelLevelOutput); 
        NewFuelConsumptionSensorOutput newFuelConsumptionOutput {lowLoopInputs.fuelConsumptionInputs.firstInput};
        bus.publish(newFuelConsumptionOutput);
        NewAirSpeedSensorOutput newAirSpeedOutput {lowLoopInputs.airSpeedInputs.firstAirSpeedInput.firstInput, loopInputs.airSpeedInputs.firstAirSpeedInput.secondInput};
        bus.publish(newAirSpeedOutput);

        newFuelLevelOutput.Output_Fuel_Level = lowLoopInputs.fuelLevelInputs.secondInput;
        bus.publish(newFuelLevelOutput);
        newFuelConsumptionOutput.Output_Fuel_Consumption =  lowLoopInputs.fuelConsumptionInputs.secondInput;
        bus.publish(newFuelConsumptionOutput);
        newAirSpeedOutput.Output_Airspeed = lowLoopInputs.airSpeedInputs.secondAirSpeedInput.secondInput;
        bus.publish(newAirSpeedOutput);
    };
    void loop(EventBus& bus) {
        NewFuelLevelSensorOutput newFuelLevelOutput {loopInputs.fuelLevelInputs.firstInput};
        bus.publish(newFuelLevelOutput); 
        NewFuelConsumptionSensorOutput newFuelConsumptionOutput {loopInputs.fuelConsumptionInputs.firstInput};
        bus.publish(newFuelConsumptionOutput);
        NewAirSpeedSensorOutput newAirSpeedOutput {loopInputs.airSpeedInputs.firstAirSpeedInput.firstInput};
        bus.publish(newAirSpeedOutput);

        newFuelLevelOutput.Output_Fuel_Level = loopInputs.fuelLevelInputs.secondInput;
        bus.publish(newFuelLevelOutput);
        newFuelConsumptionOutput.Output_Fuel_Consumption = loopInputs.fuelConsumptionInputs.secondInput;
        bus.publish(newFuelConsumptionOutput);
        newAirSpeedOutput.Output_Airspeed = loopInputs.airSpeedInputs.secondAirSpeedInput.secondInput;
        bus.publish(newAirSpeedOutput);
    };
    void recoveryLoop(EventBus& bus) {
        // NORMAL
        NewFuelLevelSensorOutput newFuelLevelOutput {loopInputs.fuelLevelInputs.firstInput};
        bus.publish(newFuelLevelOutput); 
        NewFuelConsumptionSensorOutput newFuelConsumptionOutput {loopInputs.fuelConsumptionInputs.firstInput};
        bus.publish(newFuelConsumptionOutput);
        NewAirSpeedSensorOutput newAirSpeedOutput {loopInputs.airSpeedInputs.firstAirSpeedInput.firstInput};
        bus.publish(newAirSpeedOutput);

        //LOW
        newFuelLevelOutput.Output_Fuel_Level = lowLoopInputs.fuelLevelInputs.secondInput;
        bus.publish(newFuelLevelOutput);
        newFuelConsumptionOutput.Output_Fuel_Consumption =  lowLoopInputs.fuelConsumptionInputs.secondInput;
        bus.publish(newFuelConsumptionOutput);
        newAirSpeedOutput.Output_Airspeed = lowLoopInputs.airSpeedInputs.secondAirSpeedInput.secondInput;
        bus.publish(newAirSpeedOutput);

        // CRTICAL
        newFuelLevelOutput.Output_Fuel_Level = criticalLoopInputs.fuelLevelInputs.secondInput;
        bus.publish(newFuelLevelOutput);
        newFuelConsumptionOutput.Output_Fuel_Consumption = criticalLoopInputs.fuelConsumptionInputs.secondInput;
        bus.publish(newFuelConsumptionOutput);
        newAirSpeedOutput.Output_Airspeed = criticalLoopInputs.airSpeedInputs.secondAirSpeedInput.secondInput;
        bus.publish(newAirSpeedOutput);

        // LOW
        newFuelLevelOutput.Output_Fuel_Level = lowLoopInputs.fuelLevelInputs.secondInput;
        bus.publish(newFuelLevelOutput);
        newFuelConsumptionOutput.Output_Fuel_Consumption =  lowLoopInputs.fuelConsumptionInputs.secondInput;
        bus.publish(newFuelConsumptionOutput);
        newAirSpeedOutput.Output_Airspeed = lowLoopInputs.airSpeedInputs.secondAirSpeedInput.secondInput;
        bus.publish(newAirSpeedOutput);

        // NORMAL
        newFuelLevelOutput.Output_Fuel_Level = loopInputs.fuelLevelInputs.secondInput;
        bus.publish(newFuelLevelOutput);
        newFuelConsumptionOutput.Output_Fuel_Consumption = loopInputs.fuelConsumptionInputs.secondInput;
        bus.publish(newFuelConsumptionOutput);
        newAirSpeedOutput.Output_Airspeed = loopInputs.airSpeedInputs.secondAirSpeedInput.secondInput;
        bus.publish(newAirSpeedOutput);
    }
}

TEST(EndToEndIntegrationTest, FuelLevelFunctionality) {
    EventBus bus; 
    FuelService fuelService(bus, TestConfig());
    FuelSensor fuelSensor(bus, TestConfig());
    AirSpeedSensor airSpeedSensor(bus, TestConfig());
    FuelDisplayService fuelDisplay(bus);

    // Setup subscriptions
    fuelService.Subscribe();
    fuelDisplay.Subscribe();

    int eventCount = 0;
    float testFuelLevel; 
    bus.subscribe<FuelLevelChanged>([&](const auto& event) { testFuelLevel = event.New_Fuel_Level; ++eventCount; });
    FuelLevelChanged fuelLevelChanged {75.0f, 100.0f}; 
    std::ostringstream oss; // Set up oss to capture output stream
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    std::thread sensorThread(loop, std::ref(bus)); 
    sensorThread.join(); 
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("FUEL LEVEL CHANGED"), std::string::npos);
    EXPECT_EQ(testFuelLevel, loopInputs.fuelLevelInputs.secondInput); // Verify event value matches value outputted by sensor
    EXPECT_EQ(eventCount, 2);
}

TEST(EndToEndIntegrationTest, FuelConsumptionFunctionality) {
    EventBus bus; 
    FuelService fuelService(bus, TestConfig());
    FuelSensor fuelSensor(bus, TestConfig());
    AirSpeedSensor airSpeedSensor(bus, TestConfig());
    FuelDisplayService fuelDisplay(bus);

    // Setup subscriptions
    fuelService.Subscribe();
    fuelDisplay.Subscribe();

    float testFuelConsumption;
    int eventCount = 0;
    bus.subscribe<FuelConsumptionChanged>([&](const auto& event) { testFuelConsumption = event.New_Fuel_Consumption; ++eventCount; });
    std::ostringstream oss; // Set up oss to capture output stream
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    std::thread sensorThread(loop, std::ref(bus)); 
    sensorThread.join(); 
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("FUEL CONSUMPTION CHANGED"), std::string::npos);
    EXPECT_EQ(testFuelConsumption, loopInputs.fuelConsumptionInputs.secondInput); // Verify event value matches value outputted by sensor
    EXPECT_EQ(eventCount, 2);
}

TEST(EndToEndIntegrationTest, AirSpeedFunctionality) {
    EventBus bus; 
    FuelService fuelService(bus, TestConfig());
    FuelSensor fuelSensor(bus, TestConfig());
    AirSpeedSensor airSpeedSensor(bus, TestConfig());
    FuelDisplayService fuelDisplay(bus);

    // Setup subscriptions
    fuelService.Subscribe();
    fuelDisplay.Subscribe();

    float testAirSpeed;
    int eventCount = 0;
    bus.subscribe<AirSpeedChanged>([&](const auto& event) { testAirSpeed = event.New_Air_Speed; ++eventCount; });
    std::ostringstream oss; // Set up oss to capture output stream
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    std::thread sensorThread(loop, std::ref(bus)); 
    sensorThread.join(); 
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("FUEL RANGE CHANGED"), std::string::npos);
    EXPECT_EQ(testAirSpeed, loopInputs.airSpeedInputs.secondAirSpeedInput.secondInput); // Verify event value matches value outputted by sensor
    EXPECT_EQ(eventCount, 1);
}

TEST(EndToEndIntegrationTest, FuelRangeFunctionality) {
    EventBus bus;
    FuelService fuelService(bus, TestConfig());
    FuelDisplayService fuelDisplay(bus);

    // Setup subscriptions
    fuelDisplay.Subscribe();
    fuelService.Subscribe();

    std::vector<float> observedRanges;
    int eventCount = 0;
    bus.subscribe<FuelRangeChanged>([&](const auto& event) {
        observedRanges.push_back(event.New_Fuel_Range);
        ++eventCount;
    });
    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead

    // Establish the required consumption and airspeed.
    bus.publish(NewFuelConsumptionSensorOutput{loopInputs.fuelConsumptionInputs.secondInput});
    bus.publish(NewAirSpeedSensorOutput{loopInputs.airSpeedInputs.secondAirSpeedInput.secondInput, 
        loopInputs.airSpeedInputs.secondAirSpeedInput.secondInput});
    // Simulate a new polling cycle: fuel level changes to 75.
    fuelService.SetFuelRangeFlag(false);
    bus.publish(NewFuelLevelSensorOutput{loopInputs.fuelLevelInputs.firstInput});
    // Simulate another polling cycle: fuel level returns to 100.
    fuelService.SetFuelRangeFlag(false);
    bus.publish(NewFuelLevelSensorOutput{
        loopInputs.fuelLevelInputs.secondInput
    });
    std::cout.rdbuf(stringBuffer);
    std::cout << "Captured display output:\n" << oss.str() << '\n';
    for (float range : observedRanges) {
        std::cout << "Observed fuel range: " << range << '\n';
    }
    float calculatedFuelRange = (loopInputs.fuelLevelInputs.secondInput/loopInputs.fuelConsumptionInputs.secondInput)*
        loopInputs.airSpeedInputs.secondAirSpeedInput.secondInput;

    ASSERT_FALSE(observedRanges.empty());
    EXPECT_FLOAT_EQ(observedRanges.back(), calculatedFuelRange);
    EXPECT_NE(oss.str().find("FUEL RANGE CHANGED"), std::string::npos);
    EXPECT_GE(eventCount, 3);
}



TEST(EndToEndIntegrationTest, LowFuelWarningFunctionality) {
    EventBus bus; 
    FuelService fuelService(bus, TestConfig());
    FuelSensor fuelSensor(bus, TestConfig());
    AirSpeedSensor airSpeedSensor(bus, TestConfig());
    FuelDisplayService fuelDisplay(bus);

    // Setup subscriptions
    fuelService.Subscribe();
    fuelDisplay.Subscribe();

    float testAirSpeed;
    int eventCount = 0;
    bus.subscribe<AirSpeedChanged>([&](const auto& event) { testAirSpeed = event.New_Air_Speed; ++eventCount; });
    std::ostringstream oss; // Set up oss to capture output stream
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    std::thread sensorThread(lowFuelLoop, std::ref(bus)); 
    sensorThread.join(); 
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("LOW"), std::string::npos);
    EXPECT_EQ(eventCount, 1);
}

TEST(EndToEndIntegrationTest, CriticalFuelWarningFunctionality) {
    EventBus bus; 
    FuelService fuelService(bus, TestConfig());
    FuelSensor fuelSensor(bus, TestConfig());
    AirSpeedSensor airSpeedSensor(bus, TestConfig());
    FuelDisplayService fuelDisplay(bus);

    // Setup subscriptions
    fuelService.Subscribe();
    fuelDisplay.Subscribe();

    float testAirSpeed;
    int eventCount = 0;
    bus.subscribe<AirSpeedChanged>([&](const auto& event) { testAirSpeed = event.New_Air_Speed; ++eventCount; });
    std::ostringstream oss; // Set up oss to capture output stream
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf()); // Set buffer to output into oss buffer instead
    std::thread sensorThread(criticalFuelLoop, std::ref(bus)); 
    sensorThread.join(); 
    std::cout.rdbuf(stringBuffer); // reset to normal terminal output
    EXPECT_NE(oss.str().find("CRITICAL"), std::string::npos);
    EXPECT_EQ(eventCount, 1);
    }

TEST(EndToEndIntegrationTest, WarningRecoveryFunctionality) {
    EventBus bus;
    FuelService fuelService(bus, TestConfig());
    FuelDisplayService fuelDisplay(bus);
    AirSpeedSensor airSpeedSensor(bus, TestConfig());
    FuelSensor fuelSensor(bus, TestConfig()); 

    // Set up subscriptions
    fuelService.Subscribe();
    fuelDisplay.Subscribe();

    bus.subscribe<LowFuelWarningRaised>([&](const auto& event) {} );
    bus.subscribe<LowFuelWarningCleared>([&](const auto& event) {} );
    bus.subscribe<CriticalFuelWarningRaised>([&](const auto& event) {} );
    bus.subscribe<CriticalFuelWarningCleared>([&](const auto& event) {} );

    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf());
    // Input relevant data 
    std::thread sensorLoop(recoveryLoop, std::ref(bus));
    sensorLoop.join(); 
    // Find relevant values in output terminal
    std::cout.rdbuf(stringBuffer);
    EXPECT_NE(oss.str().find("WARNING CLEARED: FUEL NORMAL"), std::string::npos);
    EXPECT_NE(oss.str().find("CRITICAL WARNING CLEARED: FUEL LOW"), std::string::npos);
}

TEST(EndToEndIntegrationTest, SuccessfulInputsFromSitlSensors) {
    EventBus bus;
    FuelService fuelService(bus, TestConfig());
    FuelDisplayService fuelDisplay(bus);
    AirSpeedSensor airSpeedSensor(bus, TestConfig());
    FuelSensor fuelSensor(bus, TestConfig()); 

    // Set up subscriptions
    fuelService.Subscribe();
    fuelDisplay.Subscribe();

    std::ostringstream oss;
    auto* stringBuffer = std::cout.rdbuf(std::cout.rdbuf()); 
    std::cout.rdbuf(oss.rdbuf());
    // Trigger getData functions
    airSpeedSensor.GetData();
    fuelSensor.GetData();
    // Find relevant values in output terminal
    std::cout.rdbuf(stringBuffer);
    EXPECT_NE(oss.str().find("FUEL CONSUMPTION CHANGED"), std::string::npos);
    EXPECT_NE(oss.str().find("FUEL RANGE CHANGED"), std::string::npos);
}