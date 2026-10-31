#include <string>
#include <vector>
#include <cmath>
#include <optional>
#include <algorithm>
#include "src/sensor-processing/sensor-processing-service.h"
#include "src/event-bus/events.h"
#include "src/event-bus/event-bus.h"


SensorProcessingService::SensorProcessingService(EventBus& bus, AircraftConfiguration aircraftConfiguration) : eventBus(bus), aircraftConfiguration(aircraftConfiguration) {
    this->Subscribe();
};

void SensorProcessingService::Subscribe() {
    this->eventBus.subscribe<NewFuelLevelSensorOutput>(
        [this](const NewFuelLevelSensorOutput& event) 
        {
            this->VerifyRawFuelLevelData(event); 
        }
    );
    this->eventBus.subscribe<NewFuelConsumptionSensorOutput>(
        [this](const NewFuelConsumptionSensorOutput& event)
        {
            this->VerifyRawFuelConsumptionData(event);
        }

    );
    this->eventBus.subscribe<NewAirSpeedSensorOutput>(
        [this](const NewAirSpeedSensorOutput& event)
        {
            this->VerifyRawAirSpeedData(event);
        }
    );
}

#include <vector>
#include <cmath>

// Validate fuel level data
void SensorProcessingService::VerifyRawFuelLevelData(const NewFuelLevelSensorOutput& rawFuelLevelData) {
    const auto& config = aircraftConfiguration.fuelMetrics;
    std::vector<float> verifiedFuelLevels;

    for (int i = 0; i < 5; i++) {
        const float fuelLevel = rawFuelLevelData.Output_Fuel_Level[i];

        const bool isValid =
            std::isfinite(fuelLevel) &&
            fuelLevel >= 0.0f &&
            fuelLevel <= config.fuel_capacity;

        if (isValid) {
            verifiedFuelLevels.push_back(fuelLevel);
        }
    }

    FuelLevelSensorVote(verifiedFuelLevels);
}


// Validate fuel consumption data
void SensorProcessingService::VerifyRawFuelConsumptionData(
    const NewFuelConsumptionSensorOutput& rawFuelConsumptionData)
{
    const auto& config = aircraftConfiguration.fuelMetrics;
    std::vector<float> verifiedFuelConsumption;

    for (int i = 0; i < 5; i++) {
        const float fuelConsumption =
            rawFuelConsumptionData.Output_Fuel_Consumption[i];

        const bool isValid =
            std::isfinite(fuelConsumption) &&
            fuelConsumption >= config.fuel_consumption_minimum &&
            fuelConsumption <= config.fuel_consumption_maximum;

        if (isValid) {
            verifiedFuelConsumption.push_back(fuelConsumption);
        }
    }

    FuelConsumptionSensorVote(verifiedFuelConsumption);
}


// Validate airspeed data
void SensorProcessingService::VerifyRawAirSpeedData(
    const NewAirSpeedSensorOutput& rawAirSpeedData)
{
    const auto& config = aircraftConfiguration.airspeeds;
    std::vector<float> verifiedAirSpeeds;

    for (int i = 0; i < 5; i++) {
        const float airSpeed = rawAirSpeedData.Output_Airspeed[i];

        const bool isValid =
            std::isfinite(airSpeed) &&
            airSpeed >= config.minimum_airspeed &&
            airSpeed <= config.maximum_airspeed;

        if (isValid) {
            verifiedAirSpeeds.push_back(airSpeed);
        }
    }

    AirSpeedSensorVote(verifiedAirSpeeds);
}