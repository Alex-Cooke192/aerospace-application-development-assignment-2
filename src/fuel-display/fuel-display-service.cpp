#include "fuel-display-service.h"
#include "event-bus/events.h"
#include <iostream>

FuelDisplayService::FuelDisplayService(EventBus& eventBus) : eventBus(eventBus) {
    this->Subscribe(); 
}

// Subscriptions to events
void FuelDisplayService::Subscribe() {
    eventBus.subscribe<FuelLevelChanged>(
        [this](const FuelLevelChanged& event)
        {
            this->PrintFuelLevelChanged(event.Original_Fuel_Level, event.New_Fuel_Level);
        }
    );
    
    eventBus.subscribe<LowFuelWarningRaised>(
        [this](const LowFuelWarningRaised& event)
        {
            this->PrintLowFuelWarning(event.Fuel_Level); 
        }
    );

    eventBus.subscribe<LowFuelWarningCleared>(
        [this](const LowFuelWarningCleared& event)
        {
            this->PrintLowFuelClear(event.Fuel_Level);
        }
    );

    eventBus.subscribe<CriticalFuelWarningCleared>(
        [this](const CriticalFuelWarningCleared& event)
        {
            this->PrintCriticalFuelWarningCleared(event.Fuel_Level);
        }
    );

    eventBus.subscribe<CriticalFuelWarningRaised>(
        [this](const CriticalFuelWarningRaised& event)
        {
            this->PrintCriticalFuelWarning(event.Fuel_Level);
        }
    );

    eventBus.subscribe<FuelConsumptionChanged>(
        [this](const FuelConsumptionChanged& event)
        {
            this->PrintFuelConsumptionChanged(event.Original_Fuel_Consumption, event.New_Fuel_Consumption);
        }
    );

    eventBus.subscribe<FuelRangeChanged>(
        [this](const FuelRangeChanged& event)
        {
            this->PrintRangeChanged(event.Original_Fuel_Range, event.New_Fuel_Range);
        }
    );

};

void FuelDisplayService::PrintFuelLevelChanged(float Original_Fuel_Level, float New_Fuel_Level) {
    std::cout << "---FUEL LEVEL CHANGED--- " << std::endl; 
    std::cout << "Original: " << std::to_string(Original_Fuel_Level) << std::endl;
    std::cout << "New:" << std::to_string(New_Fuel_Level) << std::endl; 
    std::cout << std::endl;
}

void FuelDisplayService::PrintFuelConsumptionChanged(float Original_Fuel_Consumption, float New_Fuel_Consumption) {
    std::cout << "---FUEL CONSUMPTION CHANGED---" << std::endl; 
    std::cout << "Original: " << std::to_string(Original_Fuel_Consumption) << std::endl;
    std::cout << "New:" << std::to_string(New_Fuel_Consumption) << std::endl; 
        std::cout << std::endl;
}

void FuelDisplayService::PrintRangeChanged(float Original_Range, float New_Range) {
    std::cout << "---FUEL RANGE CHANGED---" << std::endl; 
    std::cout << "Original: " << std::to_string(Original_Range) << std::endl;
    std::cout << "New:" << std::to_string(New_Range) << std::endl; 
    std::cout << std::endl;
}

void FuelDisplayService::PrintLowFuelWarning(float Fuel_Level) {
    std::cout << std::endl;
    std::cout << "------ WARNING: FUEL LOW ------" << std::endl;
    std::cout << "Fuel level: " << Fuel_Level << std::endl; 
    std::cout << "-------------------------------" << std:: endl;
    std::cout << std::endl;
}

void FuelDisplayService::PrintLowFuelClear(float Fuel_Level) {
    std::cout << std::endl;
    std::cout << "------ WARNING CLEARED: FUEL NORMAL ------" << std::endl; 
    std::cout << "Fuel level: " << Fuel_Level << std::endl; 
    std::cout << "------------------------------------------" << std:: endl;
    std::cout << std::endl;
}

void FuelDisplayService::PrintCriticalFuelWarning(float Fuel_Level) {
    std::cout << std::endl;
    std::cout << "------ WARNING: FUEL CRITICAL ------" << std::endl; 
    std::cout << "Fuel level: " << Fuel_Level << std::endl; 
    std::cout << "------------------------------------" << std:: endl;
    std::cout << std::endl;
}

void FuelDisplayService::PrintCriticalFuelWarningCleared(float Fuel_Level) {
    std::cout << std::endl;
    std::cout << "------ CRITICAL WARNING CLEARED: FUEL LOW ------" << std::endl; 
    std::cout << "Fuel level: " << Fuel_Level << std::endl; 
    std::cout << "------------------------------------------------" << std:: endl;
    std::cout << std::endl;
}