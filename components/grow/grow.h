//
// Created by necoh on 2022/09/09.
//

#ifndef BUDBROKER_SOIL_IOT_GROW_H
#define BUDBROKER_SOIL_IOT_GROW_H

#include "bmx280.h"

struct GrowMeasurement{
    float temperature;
    float humidity;
    float air_pressure;
    float vpd;
    float ambient_light;
    int soil_moisture;
    float dew_point;
};

bmx280_t* init_grow_sensors();
void measure_environmentals(bmx280_t* bmx280, struct GrowMeasurement* measurement);

#endif //BUDBROKER_SOIL_IOT_GROW_H
