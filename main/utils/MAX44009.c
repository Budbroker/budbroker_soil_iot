#include "MAX44009.h"
#include "esp_log.h"
#include "math.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#define SDA_PIN GPIO_NUM_18
#define SCL_PIN GPIO_NUM_19
#define tag "MAX44009"

#define MAX44009_ADDRESS1 0x4A
#define MAX44009_ADDRESS2 0x4B

#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1


void i2c_master_init()
{
    i2c_config_t i2c_config = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = SDA_PIN,
            .scl_io_num = SCL_PIN,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = 400000
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
        ESP_LOGE(tag, "fail to read from sensor. code: %.2X", espErr);
        return LIGHT_READ_ERROR;
    }
}