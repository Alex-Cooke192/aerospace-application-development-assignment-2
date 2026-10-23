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
    if (this->Air_Speed == 0.0) {
        // Generate a random air speed between the minimum and maximum values
        this->Air_Speed = minimum + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maximum - minimum)));
    } else {
        // Airspeed already set so use that to form the new value
        float Air_Speed_Diff = this->Air_Speed_Variance*(minimum + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maximum - minimum))));
        if (Air_Speed_Diff > this->Maximum_Air_Speed_Change) {
            Air_Speed_Diff = Maximum_Air_Speed_Change;
        }
        // Either add or subtract the difference
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::bernoulli_distribution dist(0.5);

        if (dist(gen)) {
            this->Air_Speed = this->Air_Speed+Air_Speed_Diff;
        } else {
            this->Air_Speed = this->Air_Speed-Air_Speed_Diff;
        }
    }
    bus.publish(NewAirSpeedSensorOutput{this->Air_Speed});
}

void AirSpeedSensor::PrintData() {
    // Print the current air speed
    std::cout << "Current Air Speed: " << Air_Speed << std::endl;
}