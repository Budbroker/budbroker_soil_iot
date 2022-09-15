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
#include <stdint.h>
#include "flash_storage.h"

int airValue = 4095;   //you need to replace this value
int waterValue = 2115;  //you need to replace this value
int soilMoistureValue = 0;
int soilMoisturePercent = 0;

/*
 * SOIL SENSOR DATA
 */

/**
 * Get the saved calibration numbers for the soil sensor
 */
void getSoilMonitorCalibration(){
    int savedAir = get_stored_int_value(SOIL_MONITOR_AIR);
    if(savedAir != 0){
        airValue = savedAir;
    }

    int savedWater = get_stored_int_value(SOIL_MONITOR_WATER);
    if(savedWater != 0){
        waterValue = savedWater;
    }
}

/**
 * Copied this method from arduino source code
 *
 * Re-maps a number from one range to another. That is, a value of fromLow would get mapped to toLow, a value of fromHigh
 * to toHigh, values in-between to values in-between, etc.
 *
 * @param x
 * @param in_min
 * @param in_max
 * @param out_min
 * @param out_max
 * @return
 */
long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * Read the soil moisture sensor and calculate the soil moisture percentage
 */
void readSoilMoisture(){
    soilMoistureValue = adc1_get_raw(ADC1_CHANNEL_6);
//    printf("soil moisture value %.1d\n", soilMoistureValue);
    soilMoisturePercent = map(soilMoistureValue, airValue, waterValue, 0, 100);
}

/**
 * Take a 5 sample readings from the soil moisture sensor and work out an average
 * @return int average sample
 */
int soilSensorSampling(){
    int aggregate = 0;
    int counter = 0;

    // Take 5 samples and calculate the average
    while (counter < 5){
        aggregate += aggregate + adc1_get_raw(ADC1_CHANNEL_6);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        counter++;
    }

    return aggregate/5;
}

/**
 * Save the soil moisture air sample to the onboard storage.
 */
void calibrateSoilMoistureAir(){
    int airSample = soilSensorSampling();
    set_int_storage_value(SOIL_MONITOR_AIR, airSample);
}

/**
 * Save the soil moisture water sample to the onboard storage.
 */
void calibrateSoilMoistureWater(){
    int waterSample = soilSensorSampling();
    set_int_storage_value(SOIL_MONITOR_WATER, waterSample);
}

/*
 * GENERAL ENVIRONMENTAL CALCULATIONS
 */

/**
 * Calculate the dew point using the Magnus-Tetens formula
 * @param temperature
 * @param humidity
 * @return float of the dew point temperature in deg C
 */
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

/**
 * Calculate the VPD (Vapor Pressure Deficit)
 * @param temperature
 * @param humidity
 * @return float of the VPD in Kilo Pascals (kPa)
 */
float calculateVPD(const float temperature, const float humidity)
{
    //SVP = 610.78 x e^(T / (T +237.3) x 17.2694)
    float svp = 610.78 * exp(temperature / (temperature + 237.3) * 17.2694);
    //SVP x (1 – RH/100) = VPD
    //svp is divided by 1000 to get vpd in kPa
    float vpd = (svp/1000) * (1 - humidity/100);
    return vpd;
}

/*
 * MEASUREMENT
 */

/**
 * Using all onboard sensors measure the environmentals
 * @param bmx280
 * @param measurement
 */
void measure_environmentals(bmx280_t* bmx280, struct GrowMeasurement* measurement){
    float pres = 0;
    readSoilMoisture();
    if (read_bmx(bmx280, &measurement->temperature, &pres, &measurement->humidity ) == ESP_OK){
        measurement->vpd = calculateVPD(measurement->temperature, measurement->humidity);
        measurement->dew_point = calculateDewPoint(measurement->temperature, measurement->humidity);
        measurement->ambient_light = read_ambient_light();
        measurement->air_pressure = (pres/1000);
        measurement->soil_moisture = soilMoisturePercent;
    }else{
        printf("Could not read data from sensor\n");
    }
}

/*
 * INIT THE GROW SENSOR
 */

/**
 * Master init the sensors on the device
 * @return
 */
bmx280_t* init_grow_sensors(){
    // Start up the I2C comms
    i2c_master_init();

    // Config the analog to digital comms
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
    getSoilMonitorCalibration(); //Get the saved calibration values


    //Init the BME280 sensor
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