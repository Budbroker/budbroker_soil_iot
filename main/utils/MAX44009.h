#ifndef MAX44009
#define MAX44009

#define LIGHT_READ_ERROR -1.0

void i2c_master_init();
float read_ambient_light();

#endif