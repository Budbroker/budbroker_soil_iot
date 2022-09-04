idf_component_register(
    SRCS "bluetooth.c"
    INCLUDE_DIRS "bluetooth"
    PRIV_REQUIRES nvs_flash esp_bt
)