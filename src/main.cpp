#include <iostream>
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "sitl-sensors/fuel-sensor.h"
#include "sitl-sensors/air-speed-sensor.h"
#include "event-bus/event-bus.h"
#include "fuel/fuel-service.h"
#include "sensor-health/sensor-health-service.h"
#include "event-bus/events.h"
#include "fuel-display/fuel-display-service.h"
#include "aircraft-configuration.h"



int main()
{
    EventBus eventBus;
    AircraftConfiguration aircraftConfig;
    FuelSensor fuelSensor(eventBus, aircraftConfig);
    AirSpeedSensor airSpeedSensor(eventBus, aircraftConfig);
    FuelService fuelService(eventBus, aircraftConfig); 
    SensorHealthService sensorHealthService(eventBus);
    FuelDisplayService fuelDisplay(eventBus); 

    // Initialise & connect logger
    auto logger = spdlog::basic_logger_mt("fuel_logger", "fuel_system.log");

    logger->flush_on(spdlog::level::info);

    logger->info("Fuel Management System started");

    bool running = true;

    std::thread sensorThread([&]() {
        while (running) {
            fuelService.SetFuelRangeFlag(false);

            logger->info("Updating Sensor Readings...");
            std::cout << "=========FUEL LOG=========" << std::endl;
            airSpeedSensor.GetData();
            fuelSensor.GetData();
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            // Add some gaps to make logs more readable
            std::cout << std::endl << std::endl;
        }
    });

    while (running) {
        std::string command;
        std::getline(std::cin, command);

        if (command == "quit") {
            logger->info("User Requested Shutdown");
            running = false;
        }
    }
}