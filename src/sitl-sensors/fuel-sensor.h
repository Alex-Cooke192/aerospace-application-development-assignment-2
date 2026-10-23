#pragma once

#include "sensor.h"
#include "event-bus/event-bus.h"
#include "aircraft-configuration.h"

class FuelSensor : public Sensor {
public:
    FuelSensor(EventBus& bus, AircraftConfiguration aircraftConfig);
    int GetData() override;

    float GetFuelLevelData(float minimum, float maximum); 
    float GetFuelConsumptionData(float minimum, float maximum);
    void PrintData() override;
private:
    float Fuel_Level = 0.0;
    float Fuel_Consumption = 0.0;
    EventBus& bus; 
    float _Fuel_Variance = 0.002; 
    float _Maximum_Fuel_Level_Change = 1.0;
    float _Fuel_Level_Minimum = 0.0; // Default Value: Kg
    float _Fuel_Level_Maximum = 100.0; // Default Value: Kg
    float _Fuel_Consumption_Minimum = 0.0; // Default Value: Kg per second
    float _Fuel_Consumption_Maximum = 1.0; // Default value: Kg per second 
};
