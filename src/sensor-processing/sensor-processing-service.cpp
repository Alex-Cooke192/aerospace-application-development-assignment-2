#include <string>
#include <vector>
#include <cmath>
#include <optional>
#include <algorithm>
#include "sensor-processing/sensor-processing-service.h"
#include "event-bus/events.h"
#include "event-bus/event-bus.h"


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

// Fuel level voting
std::optional<float> SensorProcessingService::FuelLevelSensorVote(
    const std::vector<float>& readings)
{
    const float tolerance =
        aircraftConfiguration.fuelMetrics.fuel_capacity *
        aircraftConfiguration.sensorMetrics.fuel_sensor_variability;

    if (readings.size() < 3) return std::nullopt;

    std::vector<float> sorted = readings;
    std::sort(sorted.begin(), sorted.end());

    for (size_t i = 0; i + 2 < sorted.size(); i++) {
        if (sorted[i + 2] - sorted[i] <= tolerance) {
            std::vector<float> accepted = {
                sorted[i], sorted[i + 1], sorted[i + 2]
            };
            
            float validFuelLevel = FuseFuelLevelSensorData(accepted);
            ValidatedFuelLevelOutput validFuelLevelEvent = {validFuelLevel};
            eventBus.publish<ValidatedFuelLevelOutput>(validFuelLevelEvent);
        }
    }

    return std::nullopt;
}


// Fuel consumption voting
std::optional<float> SensorProcessingService::FuelConsumptionSensorVote(
    const std::vector<float>& readings)
{
    const float tolerance =
        aircraftConfiguration.fuelMetrics.fuel_consumption_maximum *
        aircraftConfiguration.sensorMetrics.fuel_sensor_variability;

    if (readings.size() < 3) return std::nullopt;

    std::vector<float> sorted = readings;
    std::sort(sorted.begin(), sorted.end());

    for (size_t i = 0; i + 2 < sorted.size(); i++) {
        if (sorted[i + 2] - sorted[i] <= tolerance) {
            std::vector<float> accepted = {
                sorted[i], sorted[i + 1], sorted[i + 2]
            };
            
            float validFuelConsumption = FuseFuelConsumptionSensorData(accepted);
            ValidatedFuelConsumptionOutput validFuelConsumptionEvent = {validFuelConsumption};
            eventBus.publish<ValidatedFuelConsumptionOutput>(validFuelConsumptionEvent);
        }
    }

    return std::nullopt;
}


// Airspeed voting
std::optional<float> SensorProcessingService::AirSpeedSensorVote(
    const std::vector<float>& readings)
{
    const float tolerance =
        aircraftConfiguration.airspeeds.maximum_airspeed *
        aircraftConfiguration.sensorMetrics.air_speed_sensor_variability;

    if (readings.size() < 3) return std::nullopt;

    std::vector<float> sorted = readings;
    std::sort(sorted.begin(), sorted.end());

    for (size_t i = 0; i + 2 < sorted.size(); i++) {
        if (sorted[i + 2] - sorted[i] <= tolerance) {
            std::vector<float> accepted = {
                sorted[i], sorted[i + 1], sorted[i + 2]
            };

            float validAirSpeed = FuseAirSpeedSensorData(accepted);
            ValidatedAirSpeedOutput validAirSpeedEvent = {validAirSpeed};
            eventBus.publish<ValidatedAirSpeedOutput>(validAirSpeedEvent);
            return FuseAirSpeedSensorData(accepted);
        }
    }
    return std::nullopt;
}

float SensorProcessingService::FuseFuelLevelSensorData(const std::vector<float>& readings) {
    return readings[1];
}

float SensorProcessingService::FuseFuelConsumptionSensorData(const std::vector<float>& readings) {
    return readings[1];
}

float SensorProcessingService::FuseAirSpeedSensorData(const std::vector<float>& readings) {
    return readings[1];
}

