idf_component_register(
    SRCS "main.c" "utils/environmentals.c" "utils/MAX44009.c" "data/flash_storage.c"
    INCLUDE_DIRS "." 
    REQUIRES dht nvs_flash
    )
