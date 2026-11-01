#include "fuel-sensor.h"
#include "event-bus/events.h"
#include <stdlib.h> 
#include <iostream>
#include <random>

FuelSensor::FuelSensor(EventBus& bus, AircraftConfiguration aircraftConfig) : bus(bus) {
    this->_Fuel_Consumption_Maximum = aircraftConfig.fuelMetrics.fuel_consumption_maximum;
    this->_Fuel_Consumption_Minimum = aircraftConfig.fuelMetrics.fuel_consumption_minimum;
    this->_Fuel_Level_Maximum = aircraftConfig.fuelMetrics.fuel_capacity;
    this->_Fuel_Level_Minimum = 0.0;
}

int FuelSensor::GetData() {
    GetFuelLevelData(this->_Fuel_Level_Minimum, this->_Fuel_Level_Maximum); 
    GetFuelConsumptionData(this->_Fuel_Consumption_Minimum, this->_Fuel_Consumption_Maximum); 
    return 0;
}

void FuelSensor::GetFuelLevelData(float minimum, float maximum) {
    NewFuelLevelSensorOutput newFuelLevelSensorOutput;
    if (Fuel_Level == 0.0f) {
        Fuel_Level = minimum +
            static_cast<float>(rand()) / RAND_MAX *
            (maximum - minimum);
    } else {
        Fuel_Level -= _Maximum_Fuel_Level_Change;
    }

    Fuel_Level = std::clamp(Fuel_Level, minimum, maximum);

    // Simulate five sensors which are approximated around the initial value
    for (int i = 0; i < 5; i++) {
        float noise =
            (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f)
            * _Fuel_Variance * Fuel_Level;

        newFuelLevelSensorOutput.Output_Fuel_Level[i] =
            std::clamp(Fuel_Level + noise, minimum, maximum);
    }

    bus.publish(newFuelLevelSensorOutput);
}

void FuelSensor::GetFuelConsumptionData(float minimum, float maximum) {
    NewFuelConsumptionSensorOutput newFuelConsumptionSensorOutput;
    if (Fuel_Consumption == 0.0f) {
        Fuel_Consumption = minimum +
            static_cast<float>(rand()) / RAND_MAX *
            (maximum - minimum);
    } else {
        Fuel_Consumption -= _Fuel_Consumption_Maximum_Change;
    }

    Fuel_Consumption = std::clamp(Fuel_Consumption, minimum, maximum);

    // Simulate five sensors which are approximated around the initial value
    for (int i = 0; i < 5; i++) {
        float noise =
            (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f)
            * _Fuel_Consumption_Variance * Fuel_Consumption;

        newFuelConsumptionSensorOutput.Output_Fuel_Consumption[i] =
            std::clamp(Fuel_Consumption + noise, minimum, maximum);
    }

    bus.publish(newFuelConsumptionSensorOutput);
}

void FuelSensor::PrintData() {
    // Print the current fuel level
    std::cout << "Current Fuel Level: " << Fuel_Level << std::endl;
}