idf_component_register(
    SRCS "dc_motor.cpp" "servo.cpp"
    INCLUDE_DIRS "include"
    REQUIRES "esp_driver_ledc" "esp_driver_gpio"
)