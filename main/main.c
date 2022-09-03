#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include <dht.h>
#include "utils/environmentals.h"
#include "utils/networking.h"
#include "utils/MAX44009.h"
#include "ble/bluetooth.h"
#include "data/flash_storage.h"


#define DHT_GPIO GPIO_NUM_5
#define SENSOR_TYPE DHT_TYPE_DHT11


void app_main(void)
{
	i2c_master_init();
	
    float temperature, humidity;

    while (1) {

        if (dht_read_float_data(SENSOR_TYPE, DHT_GPIO, &humidity, &temperature) == ESP_OK)
            printf(
                "Humidity: %.1f%% Temp: %.1fC VPD: %.1fkPa DewPoint: %.1fC ambient light: %.1f Lux\n", 
                humidity, 
                temperature, 
                calculateVPD(temperature, humidity), 
                calculateDewPoint(temperature, humidity),
                read_ambient_light()
                );
        else
            printf("Could not read data from sensor\n");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}
