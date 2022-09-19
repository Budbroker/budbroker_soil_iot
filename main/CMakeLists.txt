idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "." 
    REQUIRES "nvs_flash" "bt" "bmx280" "i2c_sensors" "grow" "data" "bud_ble" "utils" "wifi"
)
