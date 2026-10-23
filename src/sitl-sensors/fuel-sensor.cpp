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

float FuelSensor::GetFuelLevelData(float minimum, float maximum) {
    if (this->Fuel_Level== 0.0) {
        // Generate a random fuel level between the minimum and maximum values
        this->Fuel_Level = minimum + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maximum - minimum)));
    } else {
        // Airspeed already set so use that to form the new value
        float Fuel_Level_Diff = this->_Fuel_Variance*(minimum + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maximum - minimum))));
        if (Fuel_Level_Diff > this->_Maximum_Fuel_Level_Change) {
            Fuel_Level_Diff = _Maximum_Fuel_Level_Change;
        }
        // Either add or subtract the difference
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::bernoulli_distribution dist(0.05); // 95% chance of fuel value going down - alot more likely! 

        if (dist(gen)) {
            this->Fuel_Level = this->Fuel_Level+Fuel_Level_Diff;
        } else {
            this->Fuel_Level = this->Fuel_Level-Fuel_Level_Diff;
        }
    }
    bus.publish(NewFuelLevelSensorOutput{this->Fuel_Level});
    return Fuel_Level;
}

float FuelSensor::GetFuelConsumptionData(float minimum, float maximum) {

    float fuelConsumption = minimum + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maximum - minimum)));
    bus.publish(NewFuelConsumptionSensorOutput{fuelConsumption});
    return fuelConsumption; 
}

void FuelSensor::PrintData() {
    // Print the current fuel level
    std::cout << "Current Fuel Level: " << Fuel_Level << std::endl;
}