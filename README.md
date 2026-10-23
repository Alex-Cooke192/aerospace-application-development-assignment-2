# Fuel and Range Management System

This repository contains the Fuel & Range Management System as produced for Aerospace Application Development Assignment 1

## Development Environment & Configuration

### Programming language & Standard

### IDE, Development Container & Compiler version/configuration

### Dependencies

## Prototype design

## Sustainability Considerations

## Building & compiling the program

The `build-and-run-project.sh` file automates the integration & deployment process, and can be ran by running the following command whilst in the project root:
```bash
./run.sh
```

# Current development state/outstanding improvements/bugfixes etc.
However there are still some improvements/bugfixes which are necessary once it has been merged in:
- Currently, the text in the terminal appears to reset for each iteration (which occurs about twice a second) and makes it difficult to enter commands that are longer than a single word. I need to figure out how to separate these threads so the sensor and CLI loops are completely independent. 
- Currently, the new values for fuel level/consumption etc. in the fuel display service are being outputted as 0. The value should instead be showing the new value outputted by the sensor, so this needs fixing. 
- Integrate the Airspeed sensor into the fuel_range calculation. 
- Implement sensor health service to monitor health state of fuel/airspeed sensors

