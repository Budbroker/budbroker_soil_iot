#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "utils/networking.h"
#include "ble/bluetooth.h"
#include "data/flash_storage.h"
#include "wifi/wifi.h"
#include "bmx280.h"
#include "grow.h"

void app_main(void)
{
    // Init the NVS storage
    init_storage();

//    set_storage_value(SERVER_HOST, "example.com");
//    erase_value(SERVER_HOST);
//    erase_value(SERVER_HOST);
    char* host = get_stored_value(SERVER_HOST);
    printf("host %s\n", host);

    bmx280_t* bmx280 =  init_grow_sensors();


    while (1) {

        if(bmx280 != NULL){
            struct GrowMeasurement measurement;
            measure_environmentals(bmx280, &measurement);
            printf(
                    "Humidity: %.1f%%, Temp: %.1f°C, VPD: %.1fkPa, DewPoint: %.1f°C, ambient light: %.1f Lux, Pressure: %.1fkPa\n",
                    measurement.humidity,
                    measurement.temperature,
                    measurement.vpd,
                    measurement.dew_point,
                    measurement.ambient_light,
                    measurement.air_pressure
            );
        }

        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}
