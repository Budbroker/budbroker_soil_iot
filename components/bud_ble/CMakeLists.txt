idf_component_register(
    SRCS "bluetooth.c"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash bt data grow
)