#include <stdlib.h> 
#include <iostream>
#include <random>
#include "air-speed-sensor.h"
#include "event-bus/event-bus.h"
#include "event-bus/events.h"

AirSpeedSensor::AirSpeedSensor(EventBus& bus, AircraftConfiguration aircraftConfig) : bus(bus) {
    this->_Maximum_Airspeed = aircraftConfig.airspeeds.maximum_airspeed;
    this->_Minimum_Airspeed = aircraftConfig.airspeeds.minimum_airspeed;
    this->Maximum_Air_Speed_Change = aircraftConfig.airspeeds.maximum_airspeed_change;
    this->Air_Speed_Variance = aircraftConfig.airspeeds.airspeed_variance;
}

int AirSpeedSensor::GetData() {
    GetAirSpeedData(this->_Minimum_Airspeed, this->_Maximum_Airspeed); 
    return 0; 
};

void AirSpeedSensor::GetAirSpeedData(float minimum, float maximum) {
    NewAirSpeedSensorOutput newAirSpeedSensorOutput;
    if (Air_Speed == 0.0f) {
        Air_Speed = minimum +
            static_cast<float>(rand()) / RAND_MAX *
            (maximum - minimum);
    } else {
        Air_Speed -= Maximum_Air_Speed_Change;
    }

    Air_Speed = std::clamp(Air_Speed, minimum, maximum);

    // Simulate five sensors which are approximated around the initial value
    for (int i = 0; i < 5; i++) {
        float noise =
            (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f)
            * Air_Speed_Variance * Air_Speed;

        newAirSpeedSensorOutput.Output_Airspeed[i] =
            std::clamp(Air_Speed + noise, minimum, maximum);
    }

    bus.publish(newAirSpeedSensorOutput);
}

void AirSpeedSensor::PrintData() {
    // Print the current air speed
    std::cout << "Current Air Speed: " << Air_Speed << std::endl;
}