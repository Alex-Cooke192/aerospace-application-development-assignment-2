#include "sensor-health-service.h"
#include "events.h"

void SensorHealthService::RaiseSensorFaultWarning() {
    bus.publish(FuelSensorFaultDetected{});
}

void SensorHealthService::ClearSensorFaultWarning() {
    bus.publish(FuelSensorFaultCleared{});  
}