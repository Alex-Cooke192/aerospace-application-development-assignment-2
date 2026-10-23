// Events.hpp
#include "fuel/fuel-state.h"

#pragma once

struct NewFuelLevelSensorOutput
{
    float Output_Fuel_Level;
};

struct NewFuelConsumptionSensorOutput
{
    float Output_Fuel_Consumption;
};

struct NewAirSpeedSensorOutput
{
    float Output_Airspeed;
    float Original_Air_Speed;
};

struct FuelLevelChanged
{
    float Original_Fuel_Level;
    float New_Fuel_Level;
};

struct FuelConsumptionChanged
{
    float Original_Fuel_Consumption;
    float New_Fuel_Consumption;
};

struct AirSpeedChanged
{
    float Original_Air_Speed;
    float New_Air_Speed;
};

struct FuelRangeChanged
{
    float Original_Fuel_Range;
    float New_Fuel_Range;
};

struct LowFuelWarningRaised
{
    float Fuel_Level; 
};

struct LowFuelWarningCleared
{
    float Fuel_Level; 
};

struct CriticalFuelWarningRaised
{
    float Fuel_Level;
};

struct CriticalFuelWarningCleared
{
    float Fuel_Level;
};

struct FuelSensorFaultDetected
{
};

struct FuelSensorFaultCleared
{
}; 