idf_component_register(
        SRCS "grow.c"
        INCLUDE_DIRS "."
        REQUIRES bmx280 i2c_sensors data
)