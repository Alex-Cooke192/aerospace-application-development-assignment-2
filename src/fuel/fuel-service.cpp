#include "fuel-service.h"
#include "event-bus/events.h"
#include "fuel-state.h"
#include "aircraft-configuration.h"

FuelService::FuelService(EventBus& bus, AircraftConfiguration aircraftConfig) : bus(bus) {
    this->Subscribe();
    this->Low_Fuel_Warning_Threshold = aircraftConfig.fuelThresholds.low_fuel_warning_threshold;
    this->Critical_Fuel_Warning_Threshold = aircraftConfig.fuelThresholds.critical_fuel_warning_threshold;
    this->FUEL_CAPACITY = aircraftConfig.fuelMetrics.fuel_capacity;
    this->Fuel_Level = aircraftConfig.fuelMetrics.Initial_Fuel_Level;
}

// Subscriptions to other services
void FuelService::Subscribe() { 
    // Listen for new sensor outputs from the sensors
    bus.subscribe<NewFuelLevelSensorOutput>(
        [this](const NewFuelLevelSensorOutput& event)
        {
            this->CheckFuelLevelChanged(event.Output_Fuel_Level); 
        }
    );
    bus.subscribe<NewFuelConsumptionSensorOutput>(
        [this](const NewFuelConsumptionSensorOutput& event)
        {
            this->CheckFuelConsumptionChanged(event.Output_Fuel_Consumption);
        }
    );
    bus.subscribe<NewAirSpeedSensorOutput>(
        [this](const NewAirSpeedSensorOutput& event)
        {
            this->CheckAirSpeedChanged(event.Output_Airspeed);
        }
    );
}

// -------------------------------------------------------------------------------------
// Fuel Level Changed

void FuelService::CheckFuelLevelChanged(float New_Fuel_Level) {
    if (this->Fuel_Level != New_Fuel_Level) {
        UpdateFuelLevel(New_Fuel_Level);
        if (this->fuelState == FuelState::NORMAL) {
            CheckLowFuelWarning();
            CheckCriticalFuelWarning();
        } else if (this->fuelState == FuelState::WARN) {
            CheckLowFuelWarningCleared();
            CheckCriticalFuelWarning();
        } else if (this->fuelState == FuelState::CRITICAL) {
            CheckCriticalFuelWarningCleared();
        }
    }
}

void FuelService::UpdateFuelLevel(float New_Fuel_Level) {
    // If its the first iteration, skip
    float Original_Fuel_Level = this->Fuel_Level;
    this->Fuel_Level = New_Fuel_Level;
    bus.publish(FuelLevelChanged{Original_Fuel_Level, New_Fuel_Level}); 
    UpdateFuelRange(this->Fuel_Level, this->Air_Speed, this->Fuel_Consumption);
} 

// ----------------------------------------------------------------------------------
// Air speed changed

void FuelService::CheckAirSpeedChanged(float New_Air_Speed) {
    if (this->Air_Speed != New_Air_Speed) {
        UpdateAirSpeed(New_Air_Speed);
    }
}

void FuelService::UpdateAirSpeed(float New_Air_Speed) {
    float Original_Air_Speed = this->Air_Speed;
    this->Air_Speed = New_Air_Speed;
    bus.publish(AirSpeedChanged{Original_Air_Speed, New_Air_Speed});
}

// -----------------------------------------------------------------------------------
// Fuel consumption

void FuelService::CheckFuelConsumptionChanged(float New_Fuel_Consumption) {
    if (this->Fuel_Consumption != New_Fuel_Consumption) {
        UpdateFuelConsumption(New_Fuel_Consumption);
    }
}

void FuelService::UpdateFuelConsumption(float New_Fuel_Consumption) {
    float Original_Fuel_Consumption = this->Fuel_Consumption;
    this->Fuel_Consumption = New_Fuel_Consumption; 
    bus.publish(FuelConsumptionChanged{Original_Fuel_Consumption, New_Fuel_Consumption}); 
    UpdateFuelRange(this->Fuel_Level, this->Air_Speed, this->Fuel_Consumption);
}

// -----------------------------------------------------------------------------------
// Fuel Range 

void FuelService::UpdateFuelRange(float New_Fuel_Level, float New_Air_Speed, float New_Fuel_Consumption)
{
    if (this->Fuel_Range_Changed_Flag == false) {
        float Endurance = New_Fuel_Level / New_Fuel_Consumption;
        float New_Fuel_Range = New_Air_Speed * Endurance;
        
        float Original_Fuel_Range = this->Fuel_Range; 
        this->Fuel_Range = New_Fuel_Range;

        bus.publish(FuelRangeChanged{Original_Fuel_Range, New_Fuel_Range});
        this->Fuel_Range_Changed_Flag = true;
    }
}

// -------------------------------------------------------------------------------
// Low fuel warning transitions

// CLEAR

void FuelService::CheckLowFuelWarningCleared() {
    if (this->fuelState == FuelState::WARN) {
        if (this->Fuel_Level > this->Low_Fuel_Warning_Threshold) {
            ClearLowFuelWarning();
        }
    }
}

void FuelService::ClearLowFuelWarning() {
    this->fuelState = FuelState::NORMAL;
    bus.publish(LowFuelWarningCleared{this->Fuel_Level});
}

// -------------------------------------------------------------------------
// RAISE

void FuelService::CheckLowFuelWarning() {
    if (this->fuelState == FuelState::NORMAL || this->fuelState == FuelState::CRITICAL) {
        if (this->Fuel_Level < this->Low_Fuel_Warning_Threshold) {
            RaiseLowFuelWarning();
        }
    }
}

void FuelService::RaiseLowFuelWarning() {
    this->fuelState = FuelState::WARN; 
    bus.publish(LowFuelWarningRaised{this->Fuel_Level}); 
}


// -------------------------------------------------------------------------
// Critical fuel warning transitions

// CLEAR

void FuelService::CheckCriticalFuelWarningCleared() {
    if (this->fuelState == FuelState::CRITICAL) {
        if (this->Fuel_Level > this->Critical_Fuel_Warning_Threshold) {
            // Clear Critical but move up to Low fuel warning
            ClearCriticalFuelWarning();
        }
    }
}

void FuelService::ClearCriticalFuelWarning() {
    this->fuelState = FuelState::WARN;
    bus.publish(CriticalFuelWarningCleared{this->Fuel_Level});
    bus.publish(LowFuelWarningRaised{this->Fuel_Level});
}

// -------------------------------------------------------------------------
// RAISE

void FuelService::CheckCriticalFuelWarning() {
    if (this->fuelState == FuelState::WARN || this->fuelState == FuelState::NORMAL) {
        if (this->Fuel_Level < this->Critical_Fuel_Warning_Threshold) {
            RaiseCriticalFuelWarning();
        }
    }
}

void FuelService::RaiseCriticalFuelWarning() {
    this->fuelState = FuelState::CRITICAL; 
    bus.publish(CriticalFuelWarningRaised{this->Fuel_Level}); 
}