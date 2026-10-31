#include <string>
#include <vector>
#include <src/event-bus/event-bus.h>
#include <src/event-bus/events.h>
#include ".config/aircraft-configuration.h"
#include <optional>


class SensorProcessingService {
    public:
    SensorProcessingService(EventBus& bus, AircraftConfiguration aircraftConfiguration);
    void Subscribe();

    // Data validation functions
    void VerifyRawFuelLevelData(const NewFuelLevelSensorOutput& rawFuelLevelData);
    void VerifyRawFuelConsumptionData(const NewFuelConsumptionSensorOutput& rawFuelConsumptionData);
    void VerifyRawAirSpeedData(const NewAirSpeedSensorOutput& rawAirSpeedData);
    
    // Then we do sensor votes
    std::optional<float> FuelLevelSensorVote(const std::vector<float>& readings);
    std::optional<float> FuelConsumptionSensorVote(const std::vector<float>& readings);
    std::optional<float> AirSpeedSensorVote(const std::vector<float>& readings);

    // Fuse valid data
    void FuseSensorData();

    private:
    EventBus& eventBus;
    AircraftConfiguration aircraftConfiguration;
    float Validated_Fuel_Level;
    float Validated_Fuel_Consumption;
    float Validated_Air_Speed;
};