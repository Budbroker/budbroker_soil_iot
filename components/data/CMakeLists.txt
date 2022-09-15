idf_component_register(
    SRCS "flash_storage.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES nvs_flash
)