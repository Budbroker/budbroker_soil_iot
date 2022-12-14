#include "flash_storage.h"

#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

/**
 * @brief Init the storage before attempting to read or write
 * 
 */
void init_storage()
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}


/**
 * @brief Get the string stored value from the flash storage
 * 
 * @param key value key in the storage.
 * @return char value stored or '\0' char null val if error or if the val does not exist.
 */
char* get_stored_value(char *key)
{
    size_t size = 0;
    char *val = "";

    nvs_handle_t handle;
    esp_err_t err = nvs_open("budbroker", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {

        err = nvs_get_str(handle, key, NULL, &size);
        if(size != 0){
            val = malloc(size);
            err = nvs_get_str(handle, key, val, &size);
            switch (err) {
                case ESP_OK:
                    printf("val found.\n");
                    break;
                case ESP_ERR_NVS_NOT_FOUND:
                    printf("val does not exist!\n");
                    break;
                default :
                    printf("Error (%s) reading!\n", esp_err_to_name(err));
            }
        }else{
            printf("val does not exist!\n");
        }
    }
    // Close
    nvs_close(handle);
    return val;
}

/**
 * @brief Get the integer stored value from the flash storage
 *
 * @param key value key in the storage.
 * @return char value stored or '\0' char null val if error or if the val does not exist.
 */
int get_stored_int_value(char *key)
{
    int32_t val = 0;
    nvs_handle_t handle;
    esp_err_t err = nvs_open("budbroker", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        err = nvs_get_i32(handle, key, &val);
        if(val != 0){
            switch (err) {
                case ESP_OK:
                    printf("val found.\n");
                    break;
                case ESP_ERR_NVS_NOT_FOUND:
                    printf("val does not exist!\n");
                    break;
                default :
                    printf("Error (%s) reading!\n", esp_err_to_name(err));
            }
        }else{
            printf("val does not exist!\n");
        }
    }
    // Close
    nvs_close(handle);
    return val;
}

/**
 * @brief Set the k,v of the storage object
 * 
 * @param key storage key
 * @param val  storage value
 */
void set_int_storage_value(char *key, int32_t val)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("budbroker", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        err = nvs_set_i32(handle, key, val);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    }
    // Close
    nvs_close(handle);
}

/**
 * @brief Set the k,v of the storage object
 *
 * @param key storage key
 * @param val string storage value
 */
void set_storage_value(char *key, char *val)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("budbroker", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        err = nvs_set_str(handle, key, val);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    }
    // Close
    nvs_close(handle);
}

/**
 * @brief delete storage object by key
 * @param key
 */
void erase_value(char *key)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("budbroker", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        err = nvs_erase_key(handle, key);
        switch (err) {
            case ESP_OK:
                printf("Successfully deleted key.");
                break;
            default :
                printf("Error (%s) deleting!\n", esp_err_to_name(err));
        }
    }
    nvs_close(handle);
}
