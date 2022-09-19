#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

//Internal Components
#include "networking.h"
#include "bluetooth.h"
#include "flash_storage.h"
#include "wifi.h"
#include "bmx280.h"
#include "grow.h"


void measureTask(void * parameter){

    bmx280_t* bmx280 = (bmx280_t *) parameter;

    while (1) {

        if(bmx280 != NULL){
            struct GrowMeasurement measurement;
            measure_environmentals(bmx280, &measurement);
            printf(
                    "Humidity: %.1f%%, Temp: %.1f°C, VPD: %.1fkPa, DewPoint: %.1f°C, ambient light: %.1f Lux, Air Pressure: %.1fkPa Soil Moisture %%: %.1d%%\n",
                    measurement.humidity,
                    measurement.temperature,
                    measurement.vpd,
                    measurement.dew_point,
                    measurement.ambient_light,
                    measurement.air_pressure,
                    measurement.soil_moisture
            );
        }

        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    // Init the NVS storage
    init_storage();
    bmx280_t* bmx280 =  init_grow_sensors();

    xTaskCreate(&measureTask, "measure", 2048, (void *) bmx280, 2, NULL);

    startBleServer();

    //Init the sensors and get the bmx280

//

}
