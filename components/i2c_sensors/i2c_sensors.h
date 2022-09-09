#ifndef MAX44009
#define MAX44009
#define LIGHT_READ_ERROR -1.0

#include "bmx280.h"

void i2c_master_init();
float read_ambient_light();
esp_err_t read_bmx(bmx280_t* bmx280, float* temp, float* pres, float* hum );

#endif