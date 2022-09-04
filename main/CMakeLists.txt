idf_component_register(
    SRCS "main.c" "utils/environmentals.c" "utils/MAX44009.c"
    INCLUDE_DIRS "." 
    REQUIRES dht
    )
add_library(environmentals utils/environmentals.c)
add_library(MAX44009 utils/MAX44009.c)