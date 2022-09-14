#include "i2c_sensors.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include <esp_system.h>
#include "math.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#include "bmx280.h"

#define SDA_PIN GPIO_NUM_19 //I2C data line on pin 19
#define SCL_PIN GPIO_NUM_18 //I2C clock line on pin 18

#define tag_light_sensor "MAX44009"
#define MAX44009_ADDRESS1 0x4A
#define MAX44009_ADDRESS2 0x4B

#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1

/**
 * Init I2C on the device
 */
void i2c_master_init()
{
    i2c_config_t i2c_config = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = SDA_PIN,
            .scl_io_num = SCL_PIN,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = 1000000
    };
    i2c_param_config(I2C_NUM_0, &i2c_config);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}


/**
 * @brief Read the ambient light in lux
 * 
 * @return float lux value of light or -1 if error
 */
float read_ambient_light(){
    uint8_t lux_h;
    uint8_t lux_l;

    esp_err_t espErr;
    i2c_cmd_handle_t cmd;

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MAX44009_ADDRESS1 << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x03, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MAX44009_ADDRESS1 << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &lux_h, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    espErr = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    // According to datasheet (p17), we can read two registers in one transmission
    // by repeated start signal. But unfortunately it timeouts.
    // So we re-create or I2C link for to get lux low-byte.
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MAX44009_ADDRESS1 << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x04, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MAX44009_ADDRESS1 << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &lux_l, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    espErr = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
    if (espErr == ESP_OK) {
        int exponent = (lux_h & 0xf0) >> 4;
        int mant = (lux_h & 0x0f) << 4 | lux_l;
        float lux = (float)(((0x00000001 << exponent) * (float)mant) * 0.045);
        i2c_cmd_link_delete(cmd);
        return lux;
    } else {
        ESP_LOGE(tag_light_sensor, "fail to read from sensor. code: %.2X", espErr);
        return LIGHT_READ_ERROR;
    }
}

/**
 * @brief Read BMP280 or BME280 sensor
 * @param bmx280
 * @param temperature
 * @param pressure
 * @param humidity
 * @return esp_err_t
 */
esp_err_t read_bmx(bmx280_t* bmx280, float* temp, float* pres, float* hum ){
    esp_err_t err = bmx280_setMode(bmx280, BMX280_MODE_FORCE);
    if (err == ESP_OK){
        do {
            vTaskDelay(pdMS_TO_TICKS(1));
        } while(bmx280_isSampling(bmx280));
    }else{
        ESP_LOGE("BME280", "fail to set sensor mode. code: %.2X", err);
        return err;
    }

    return bmx280_readoutFloat(bmx280, temp, pres, hum);
}