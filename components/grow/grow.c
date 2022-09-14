//
// Created by necoh on 2022/09/09.
//

#include "grow.h"
#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include <math.h>
#include "i2c_sensors.h"
#include "bmx280.h"
#include "driver/gpio.h"
#include <driver/adc.h>


#define SOIL_SENSOR GPIO_NUM_12 //pin 12 used for soil moisture sensor

const int AirValue = 4095;   //you need to replace this value with Value_1
const int WaterValue = 2115;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent=0;


bmx280_t* init_grow_sensors(){
    i2c_master_init();
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);

    bmx280_t* bmx280 = bmx280_create(I2C_NUM_0);
    if (!bmx280) {
        ESP_LOGE("test", "Could not create bmx280 driver.");
        return NULL;
    }

    bmx280_init(bmx280);

    bmx280_config_t bmx_cfg = BMX280_DEFAULT_CONFIG;
    esp_err_t bme_err = bmx280_configure(bmx280, &bmx_cfg);
    if(bme_err != ESP_OK){
        printf("BME280 Error");
    }

    return bmx280;
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void readSoilMoisture(){
    soilMoistureValue = adc1_get_raw(ADC1_CHANNEL_6);
//    printf("soil moisture value %.1d\n", soilMoistureValue);
    soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
}

float calculateDewPoint(const float temperature, const float humidity)
{
    int t = round(temperature);
    int h = round(humidity);
    if (t < -45 || t > 65 || h == 0) {
        return NAN;
    }

    float a = 17.62;
    float b = 243.12;

    /**
     * Magnus-Tetens formula (Sonntag90):
     *
     * Ts = (bα(T,RH)) / (a - α(T,RH))
     *
     * Ts is the dew point;
     * T is the temperature;
     * RH is the relative humidity of the air;
     * a and b are coefficients. For Sonntag90 constant set, a = 17.62
     * and b = 243.12°C;
     * α(T,RH) = ln(RH/100) + aT/(b+T)
     */
    float alpha = log(humidity / 100) + a * temperature / (b + temperature);
    float dewPoint = (b * alpha) / (a - alpha);

    return dewPoint;
}

float calculateVPD(const float temperature, const float humidity)
{
    //SVP = 610.78 x e^(T / (T +237.3) x 17.2694)
    float svp = 610.78 * exp(temperature / (temperature + 237.3) * 17.2694);
    //SVP x (1 – RH/100) = VPD
    //svp is divided by 1000 to get vpd in kPa
    float vpd = (svp/1000) * (1 - humidity/100);
    return vpd;
}

void measure_environmentals(bmx280_t* bmx280, struct GrowMeasurement* measurement){
    float pres = 0;
    readSoilMoisture();
    if (read_bmx(bmx280, &measurement->temperature, &pres, &measurement->humidity ) == ESP_OK){
        measurement->vpd = calculateVPD(measurement->temperature, measurement->humidity);
        measurement->dew_point = calculateDewPoint(measurement->temperature, measurement->humidity);
        measurement->ambient_light = read_ambient_light();
        measurement->air_pressure = (pres/1000);
        measurement->soil_moisture = soilmoisturepercent;
    }else{
        printf("Could not read data from sensor\n");
    }
}