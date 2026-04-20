idf_component_register (
    SRCS "joystick.cpp" "control.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_adc esp_driver_gpio send
)