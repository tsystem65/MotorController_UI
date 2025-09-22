cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
add_compile_options("-Wno-format")
project(MotorController_UI)

set(EXTRA_COMPONENT_DIRS 
    "uart_manager"
    "wifi_manager"
    "event_manager"
    "nvs_manager"
    "ota_manager"
    "uthash")