# RC Car - ESP-IDF Project

## Overview
RC car firmware built with ESP-IDF (C++), targeting the **ESP32-C3**. Uses LEDC peripheral for PWM control of a DC motor (L298N driver) and a servo (SG90).

## Project Structure
```
espidf_car/
  CMakeLists.txt                      # Top-level: cmake 3.31, includes IDF toolchain
  main/
    CMakeLists.txt                    # Registers app_main.cpp
    app_main.cpp                      # Entry point, configures & runs motor+servo demo loop
  components/
    drivers/
      CMakeLists.txt                  # Registers dc_motor.cpp, servo.cpp; REQUIRES esp_driver_ledc, esp_driver_gpio
      include/
        dc_motor.h                    # Driver::L298N class, MotorConfig, Direction, StopMode
        servo.h                       # Driver::Sg90 class, ServoConfig
      dc_motor.cpp                    # L298N implementation: init, set_throttle_percent, set_speed_percent, set_direction, stop
      servo.cpp                       # Sg90 implementation: init, set_angle (pulse-width calculation from angle)
```

## Hardware Pin Mapping
| Function       | GPIO |
|----------------|------|
| Motor Enable   | 10   |
| Motor IN1      | 6    |
| Motor IN2      | 7    |
| Servo PWM      | 5    |

## Key APIs

### Driver::L298N (DC Motor via L298N H-bridge)
- `init()` - configures GPIOs and LEDC timer/channel (timer 0, channel 0, 10-bit, 1kHz)
- `set_throttle_percent(int8_t)` - combined direction+speed, -100 to 100
- `set_speed_percent(uint8_t)` - 0-100 duty cycle
- `set_direction(Direction)` - Forward/Reverse via IN1/IN2
- `stop(StopMode)` - Brake (both HIGH) or Coast (both LOW)

### Driver::Sg90 (Servo)
- `init()` - configures LEDC (timer 1, channel 1, 14-bit, 50Hz), centers to 90 degrees
- `set_angle(uint8_t)` - maps angle to pulse width (1000-2000us, configurable)

## Build & Flash
```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Current State
- DC motor driver: complete and tested
- Servo driver: working, needs calibrating (pulse range currently 1000-2000us, comments suggest 500-2400us full range)
- `app_main` runs a demo loop: forward 75% + turn, stop, reverse 75% + turn, stop
- No wireless control yet (BLE/WiFi not implemented)

## Conventions
- All driver code lives in the `Driver` namespace
- Config structs are passed by const reference to constructors
- All public methods return `esp_err_t`
- Error handling uses `ESP_RETURN_ON_ERROR` (servo) or manual `if (err != ESP_OK)` checks (motor)
