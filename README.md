# smt32-htu21d-library
Library for controlling and reading temperature and humidity from htu21d sensor with STM32 MCU.
Communication is done using blocking mode which means MCU waits for I2C transmission to be finished.

# Structure

Code with I2C communication between MCU and sensor HTU21D.
- Core/Src/htu21d_sensor.c
- Core/Src/htu21d_sensor.h

# main file containing simple example
- Core/Src/main.c
