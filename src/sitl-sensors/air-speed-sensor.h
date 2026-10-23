#pragma once

#include "sensor.h"
#include "event-bus/event-bus.h"
#include "aircraft-configuration.h"

class AirSpeedSensor : public Sensor { 
public:
    // Sensors dont have subscriptions as they only output data
    AirSpeedSensor(EventBus& bus, AircraftConfiguration aircraftConfig);
    int GetData() override;
    void GetAirSpeedData(float minimum, float maximum); 
    void PrintData() override;
private:
    float Air_Speed = 0.0;
    float Maximum_Air_Speed_Change = 5.0;
    float Air_Speed_Variance = 0.03; // For the purposes of the simulation, 
                                     // this is how severaly the altitude fluctubates
    EventBus& bus; 
    float _Minimum_Airspeed = 50.0; 
    float _Maximum_Airspeed = 500.0; 
};
