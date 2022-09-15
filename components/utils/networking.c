
#include <stdio.h>
#include <string.h>
#include "networking.h"
#include "grow.h"

char *intToString(int integer){
    int length = snprintf( NULL, 0, "%d", integer );
    char* str = malloc( length + 1 );
    snprintf( str, length + 1, "%d", integer );
    return str;
}

char *floatToString(float floatingPoint){
    int length = snprintf( NULL, 0, "%g", floatingPoint );
    char *string = malloc( length + 1 );
    snprintf( string, length + 1, "%g", floatingPoint );
    return string;
}

/**
 * Turn a measurement object into json so it can be sent via a REST call to the server
 * @param json
 * @param measurement obj
 * @param growId string ID of the grow.
 */
void measurementToJson(char json[516], struct GrowMeasurement measurement, char* growId)
{
    memset(json, 0, 516);

    strcat(json, "{\"temperature\": ");
    char* temperature = floatToString(measurement.temperature);
    strcat(json, temperature);
    free(temperature);

    char* humidity = floatToString(measurement.humidity);
    strcat(json, ", \"humidity\": ");
    strcat(json, humidity);
    free(humidity);

    char* air_pressure = floatToString(measurement.air_pressure);
    strcat(json, ", \"air_pressure\": ");
    strcat(json, air_pressure);
    free(air_pressure);

    char* vpd = floatToString(measurement.vpd);
    strcat(json, ", \"vpd\": ");
    strcat(json, vpd);
    free(vpd);

    char* ambient_light = floatToString(measurement.ambient_light);
    strcat(json, ", \"ambient_light\": ");
    strcat(json, ambient_light);
    free(ambient_light);

    char* soil_moisture = intToString(measurement.soil_moisture);
    strcat(json, ", \"soil_moisture\": ");
    strcat(json, soil_moisture);
    free(soil_moisture);

    char* dew_point = floatToString(measurement.dew_point);
    strcat(json, ", \"dew_point\": ");
    strcat(json, dew_point);
    free(dew_point);

    strcat(json, ", \"grow_id\": ");
    strcat(json, "\"");
    strcat(json, growId);
    strcat(json, "\"");

    strcat(json, "}");

}



// TODO create HTTP client to post measurements to server