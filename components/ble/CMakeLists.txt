idf_component_register(
    SRCS "bluetooth.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES nvs_flash data
)