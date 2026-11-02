#include "event-bus/event-bus.h"
#include "event-bus/events.h"
#include <aircraft-configuration.h>

namespace {
    AircraftConfiguration TestConfig() {
        AircraftConfiguration config;
        
        config.fuelMetrics.Initial_Fuel_Level = 100.0f;
        config.fuelThresholds.low_fuel_warning_threshold = 20.0f;
        config.fuelThresholds.critical_fuel_warning_threshold = 10.0f;

        return config;
    };

    struct InitialFuelLevelInputs {
        float values[5];} 
    validInitialFuelLevelInputs{{200.0f, 201.0f, 199.5f, 200.5f, 199.0f}}, 
    anomalousInitialFuelLevelInputs{{200.0f, 201.0f, 199.0f, 150.0f, 245.0f}}, 
    invalidInitialFuelLevelInputs{{50.0f, 80.0f, 110.0f, 140.0f, 170.0f}};

    struct FinalFuelLevelInputs {
        float values[5];} 
    validFinalFuelLevelInputs{{198.0f, 199.0f, 197.5f, 198.5f, 197.0f}}, 
    anomalousFinalFuelLevelInputs{{198.0f, 199.0f, 197.0f, 150.0f, 245.0f}}, 
    invalidFinalFuelLevelInputs{{45.0f, 75.0f, 105.0f, 135.0f, 165.0f}};

    struct InitialFuelConsumptionInputs {
        float values[5];} 
    validInitialFuelConsumptionInputs{{0.0300f, 0.0302f, 0.0299f, 0.0301f, 0.0298f}}, 
    anomalousInitialFuelConsumptionInputs{{0.0300f, 0.0302f, 0.0299f, 0.0120f, 0.0490f}}, 
    invalidInitialFuelConsumptionInputs{{0.010f, 0.020f, 0.030f, 0.040f, 0.050f}};

    struct FinalFuelConsumptionInputs {
        float values[5];} 
    validFinalFuelConsumptionInputs{{0.0320f, 0.0322f, 0.0319f, 0.0321f, 0.0318f}}, 
    anomalousFinalFuelConsumptionInputs{{0.0320f, 0.0322f, 0.0319f, 0.0120f, 0.0490f}}, 
    invalidFinalFuelConsumptionInputs{{0.011f, 0.021f, 0.031f, 0.041f, 0.050f}};

    struct InitialAirSpeedInputs {
        float initialValues[5];
        float finalValues[5];} 
    validInitialAirSpeedInputs{{250.0f, 251.0f, 249.5f, 250.5f, 249.0f},
                                 {251.0f, 252.0f, 250.5f, 251.5f, 250.0f}}, 
    anomalousInitialAirSpeedInputs{{250.0f, 251.0f, 249.5f, 100.0f, 350.0f},
                                   {251.0f, 252.0f, 250.5f, 100.0f, 350.0f}}, 
    invalidInitialAirSpeedInputs{{100.0f, 150.0f, 200.0f, 250.0f, 300.0f},
                                 {101.0f, 151.0f, 201.0f, 251.0f, 301.0f}};


    struct FinalAirSpeedInputs {
        float initialValues[5];
        float finalValues[5];} 
    validFinalAirSpeedInputs{{251.0f, 252.0f, 250.5f, 251.5f, 250.0f},
                             {252.0f, 253.0f, 251.5f, 252.5f, 251.0f}}, 
    anomalousFinalAirSpeedInputs{{251.0f, 252.0f, 250.5f, 100.0f, 350.0f},
                                 {252.0f, 253.0f, 251.5f, 100.0f, 350.0f}}, 
    invalidFinalAirSpeedInputs{{101.0f, 151.0f, 201.0f, 251.0f, 301.0f},
                               {102.0f, 152.0f, 202.0f, 252.0f, 302.0f}};

    // Publish one batch of five readings for each sensor quantity.
    void publishSensorData(
        EventBus& bus,
        const float (&fuelLevels)[5],
        const float (&fuelConsumption)[5],
        const float (&airSpeeds)[5])
    {
        NewFuelLevelSensorOutput fuelLevelOutput{};
        NewFuelConsumptionSensorOutput fuelConsumptionOutput{};
        NewAirSpeedSensorOutput airSpeedOutput{};

        std::copy_n(fuelLevels, 5, fuelLevelOutput.Output_Fuel_Level);
        std::copy_n(fuelConsumption, 5,
                    fuelConsumptionOutput.Output_Fuel_Consumption);
        std::copy_n(airSpeeds, 5, airSpeedOutput.Output_Airspeed);

        bus.publish(fuelLevelOutput);
        bus.publish(fuelConsumptionOutput);
        bus.publish(airSpeedOutput);
    }


    // Valid sensor measurements
    void validLoop(EventBus& bus)
    {
        // Initial measurements
        publishSensorData(
            bus,
            validInitialFuelLevelInputs.values,
            validInitialFuelConsumptionInputs.values,
            validInitialAirSpeedInputs.initialValues
        );

        // Final measurements
        publishSensorData(
            bus,
            validFinalFuelLevelInputs.values,
            validFinalFuelConsumptionInputs.values,
            validFinalAirSpeedInputs.finalValues
        );
    }


    // Physically valid measurements containing outliers
    void anomalousLoop(EventBus& bus)
    {
        // Initial measurements
        publishSensorData(
            bus,
            anomalousInitialFuelLevelInputs.values,
            anomalousInitialFuelConsumptionInputs.values,
            anomalousInitialAirSpeedInputs.initialValues
        );

        // Final measurements
        publishSensorData(
            bus,
            anomalousFinalFuelLevelInputs.values,
            anomalousFinalFuelConsumptionInputs.values,
            anomalousFinalAirSpeedInputs.finalValues
        );
    }


    // Sensor measurements with no voting consensus
    void invalidLoop(EventBus& bus)
    {
        // Initial measurements
        publishSensorData(
            bus,
            invalidInitialFuelLevelInputs.values,
            invalidInitialFuelConsumptionInputs.values,
            invalidInitialAirSpeedInputs.initialValues
        );

        // Final measurements
        publishSensorData(
            bus,
            invalidFinalFuelLevelInputs.values,
            invalidFinalFuelConsumptionInputs.values,
            invalidFinalAirSpeedInputs.finalValues
        );
    }


    // Recovery: valid → anomalous → invalid → anomalous → valid
    void recoveryLoop(EventBus& bus)
    {
        // Normal operation
        publishSensorData(
            bus,
            validInitialFuelLevelInputs.values,
            validInitialFuelConsumptionInputs.values,
            validInitialAirSpeedInputs.initialValues
        );

        // Outlier rejection
        publishSensorData(
            bus,
            anomalousInitialFuelLevelInputs.values,
            anomalousInitialFuelConsumptionInputs.values,
            anomalousInitialAirSpeedInputs.initialValues
        );

        // Loss of voting consensus
        publishSensorData(
            bus,
            invalidInitialFuelLevelInputs.values,
            invalidInitialFuelConsumptionInputs.values,
            invalidInitialAirSpeedInputs.initialValues
        );

        // Consensus recovered with outliers
        publishSensorData(
            bus,
            anomalousFinalFuelLevelInputs.values,
            anomalousFinalFuelConsumptionInputs.values,
            anomalousFinalAirSpeedInputs.finalValues
        );

        // Normal operation restored
        publishSensorData(
            bus,
            validFinalFuelLevelInputs.values,
            validFinalFuelConsumptionInputs.values,
            validFinalAirSpeedInputs.finalValues
        );
    }
}