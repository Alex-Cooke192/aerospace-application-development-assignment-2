#include "event-bus/event-bus.h"

class FuelDisplayService {
    public:
    FuelDisplayService(EventBus& EventBus);
    void Subscribe(); 

    void PrintFuelLevelChanged(float Original_Fuel_Level, float New_Fuel_Level);
    void PrintLowFuelWarning(float Fuel_Level); 
    void PrintLowFuelClear(float Fuel_Level); 
    void PrintCriticalFuelWarning(float Fuel_Level);
    void PrintCriticalFuelWarningCleared(float Fuel_Level);
    void PrintFuelConsumptionChanged(float Original_Fuel_Consumption, float New_Fuel_Consumption); 
    void PrintRangeChanged(float Original_Fuel_Range, float New_Fuel_Range); 

    private:
    EventBus& eventBus; 
}; 