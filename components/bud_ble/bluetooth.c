#include <stdio.h>
#include <string.h >
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "sdkconfig.h"

#include "bluetooth.h"
#include "flash_storage.h"
#include "grow.h"

//====================================================================================================================//

void ble_app_advertise(void);

//====================================================================================================================//

char *TAG = "BudBroker-Soil-Sensor";
uint8_t ble_addr_type;
char userToken[256];
char refreshToken[256];
char serverHost[256];
char growId[256];
char ssid[256];
char wifiPass[256];
bool isReading = false;
char data[] = {0x11,0x11};

//====================================================================================================================//

/**
 * This Enum holds the command bytes that will serve as the commands from the client
 */
enum BUD_BLE_ACTIONS
{
    DONE = 0x00,
    END = 0x01,
    USER_TOKEN = 0x02,
    REFRESH_TOKEN = 0x03,
    SERVER_HOST = 0x04,
    GROW_ID = 0x05,
    WIFI_SSD = 0x06,
    WIFI_PASSWORD = 0x07,
    SOIL_AIR = 0x08,
    SOIL_WATER = 0x09,
    REBOOT = 0x50,
    RESET = 0x66,
    ERROR = 0x99
};
enum BUD_BLE_ACTIONS currentlyReading = ERROR;
enum BUD_BLE_ACTIONS lastRead = ERROR;

//================================================READ & WRITE METHODS================================================//

/**
 * Use to clear all data from the client, used when a mistake has been made and data needs to be reset
 */
void clearAllData(){
    memset(userToken, 0, 256);
    memset(refreshToken, 0, 256);
    memset(serverHost, 0, 256);
    memset(growId, 0, 256);
    memset(ssid, 0, 256);
    memset(wifiPass, 0, 256);
}

/**
 * Process the buffer received from the BLE client and return the string without the command byte.
 * @param buff
 * @param len
 * @return
 */
char * getWriteSubstring(uint8_t * buff, uint16_t len){
    char * subset = malloc((len)*sizeof(int));
    memset(subset, 0, (len)*sizeof(int));
    for(int i=1;i<len;i++){
        subset[i-1] = buff[i];
    }
    return subset;
}

/**
 * On subsequent reads of the client data we dont want to cut the first byte so use the below method te get all the data
 * @param buff
 * @param len
 * @return
 */
char * bleDataToString(uint8_t * buff, uint16_t len){
    char * subset = malloc((len)*sizeof(int));
    memset(subset, 0, (len)*sizeof(int));
    for(int i=0;i<len;i++){
        subset[i] = buff[i];
    }
    return subset;
}

/**
 * Save the values retrieved from the BLE Client to the ESP32 onboard storage.
 */
void saveParams(){
    switch (lastRead) {
        case USER_TOKEN:
//            set_storage_value(USER_ACCESS_TOKEN_KEY, userToken);
            printf("%s\n", userToken);
            ESP_LOGI("BLE", "User token saved");
            break;
        case REFRESH_TOKEN:
            set_storage_value(USER_REFRESH_TOKEN_KEY, refreshToken);
            ESP_LOGI("BLE", "Refresh token saved");
            break;
        case SERVER_HOST:
            set_storage_value(SERVER_HOST_KEY, serverHost);
            ESP_LOGI("BLE", "Host saved");
            break;
        case GROW_ID:
            set_storage_value(GROW_ID_KEY, growId);
            ESP_LOGI("BLE", "Grow ID saved");
            break;
        case WIFI_SSD:
            set_storage_value(WIFI_SSID_KEY, ssid);
            ESP_LOGI("BLE", "SSID saved");
            break;
        case WIFI_PASSWORD:
            set_storage_value(WIFI_PASSWORD_KEY, wifiPass);
            ESP_LOGI("BLE", "WIFI Password saved");
            break;
        default:
            ESP_LOGI("BLE", "Error on save");
            break;
    }
}

/**
 * Delete all stored values on the device
 */
void deviceReset(){
    erase_value(USER_ACCESS_TOKEN_KEY);
    erase_value(USER_REFRESH_TOKEN_KEY);
    erase_value(SERVER_HOST_KEY);
    erase_value(GROW_ID_KEY);
    erase_value(WIFI_SSID_KEY);
    erase_value(WIFI_PASSWORD_KEY);
    erase_value(SOIL_MONITOR_AIR_KEY);
    erase_value(SOIL_MONITOR_WATER_KEY);
}

/**
 * Restart the sensor
 */
void deviceRestart(){
    esp_restart();
}

/**
 * Recieve write data from the client
 * @param conn_handle
 * @param attr_handle
 * @param ctxt
 * @param arg
 * @return
 */
static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
//    printf("Data from the client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    char *sub;
    char *bleData;
    if (isReading){
        switch (currentlyReading) {
            case END:
                saveParams(); //client is done sending data, save it to the flash storage
                isReading = false;
                currentlyReading = END;
                lastRead = END;
                break;
            case USER_TOKEN:
                bleData = bleDataToString(ctxt->om->om_data,ctxt->om->om_len);
                strcat(userToken, bleData);
                lastRead = USER_TOKEN;
                break;
            case REFRESH_TOKEN:
                bleData = bleDataToString(ctxt->om->om_data,ctxt->om->om_len);
                strcat(refreshToken, bleData);
                lastRead = REFRESH_TOKEN;
                break;
            case SERVER_HOST:
                bleData = bleDataToString(ctxt->om->om_data,ctxt->om->om_len);
                strcat(serverHost, bleData);
                lastRead = SERVER_HOST;
                break;
            case GROW_ID:
                bleData = bleDataToString(ctxt->om->om_data,ctxt->om->om_len);
                strcat(growId, bleData);
                lastRead = GROW_ID;
                break;
            case WIFI_SSD:
                bleData = bleDataToString(ctxt->om->om_data,ctxt->om->om_len);
                strcat(ssid, bleData);
                lastRead = WIFI_SSD;
                break;
            case WIFI_PASSWORD:
                bleData = bleDataToString(ctxt->om->om_data,ctxt->om->om_len);
                strcat(wifiPass, bleData);
                lastRead = WIFI_PASSWORD;
                break;
            default:
                currentlyReading = ERROR;
                lastRead = ERROR;
                isReading = false;
                clearAllData();
                ESP_LOGI("APP", "ERROR");
                break;
        }
    } else {
        switch (ctxt->om->om_data[0]) {
            case END:
                ESP_LOGI("APP", "END");
                strcpy( data, (char[]){END,0x11} );
                isReading = false;
                currentlyReading = END;
                break;
            case USER_TOKEN:
                sub = getWriteSubstring(ctxt->om->om_data, ctxt->om->om_len);
                strcat(userToken, sub);
                ESP_LOGI("APP", "USER TOKEN");
                strcpy( data, (char[]){USER_TOKEN,0x11} );
                currentlyReading = USER_TOKEN;
                isReading = true;
                break;
            case REFRESH_TOKEN:
                sub = getWriteSubstring(ctxt->om->om_data, ctxt->om->om_len);
                strcat(refreshToken, sub);
                ESP_LOGI("APP", "REFRESH TOKEN");
                currentlyReading = REFRESH_TOKEN;
                strcpy( data, (char[]){REFRESH_TOKEN,0x11} );
                isReading = true;
                break;
            case SERVER_HOST:
                sub = getWriteSubstring(ctxt->om->om_data, ctxt->om->om_len);
                strcat(serverHost, sub);
                ESP_LOGI("APP", "HOST");
                strcpy( data, (char[]){SERVER_HOST,0x11} );
                currentlyReading = SERVER_HOST;
                isReading = true;
                break;
            case GROW_ID:
                sub = getWriteSubstring(ctxt->om->om_data, ctxt->om->om_len);
                strcat(growId, sub);
                ESP_LOGI("APP", "GROW ID");
                strcpy( data, (char[]){GROW_ID,0x11} );
                currentlyReading = GROW_ID;
                isReading = true;
                break;
            case WIFI_SSD:
                sub = getWriteSubstring(ctxt->om->om_data, ctxt->om->om_len);
                strcat(ssid, sub);
                ESP_LOGI("APP", "WIFI SSD");
                strcpy( data, (char[]){WIFI_SSD,0x11} );
                currentlyReading = WIFI_SSD;
                isReading = true;
                break;
            case WIFI_PASSWORD:
                sub = getWriteSubstring(ctxt->om->om_data, ctxt->om->om_len);
                strcat(wifiPass, sub);
                ESP_LOGI("APP", "WIFI SSD");
                strcpy( data, (char[]){WIFI_PASSWORD,0x11} );
                currentlyReading = WIFI_PASSWORD;
                isReading = true;
                break;
            case SOIL_AIR:
                ESP_LOGI("APP", "SOIL AIR");
                calibrateSoilMoistureAir(); // Do the device calibration
                strcpy( data, (char[]){DONE,0x11} ); //Let the client know that the calibration is done
                currentlyReading = END;
                break;
            case SOIL_WATER:
                ESP_LOGI("APP", "SOIL WATER");
                calibrateSoilMoistureWater(); // Do the device calibration
                strcpy( data, (char[]){DONE,0x11} ); //Let the client know that the calibration is done
                currentlyReading = END;
                break;
            case REBOOT:
                ESP_LOGI("APP", "REBOOT");
                currentlyReading = END;
                deviceRestart();
                break;
            case RESET:
                ESP_LOGI("APP", "RESET");
                currentlyReading = END;
                deviceReset();
                break;
            default:
                currentlyReading = ERROR;
                clearAllData();
                strcpy( data, (char[]){ERROR,0x11} );
                isReading = false;
                ESP_LOGI("APP", "ERROR");
                break;
        }
    }
    return 0;
}

/**
 * Send the read data, mostly just used for debugging to see what process is currently underway
 * @param con_handle
 * @param attr_handle
 * @param ctxt
 * @param arg
 * @return
 */
static int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, data, 2);
    return 0;
}

/**
 * When the client is done sending data it needs to read this characteristic to indicate that it is done sending data.
 * @param con_handle
 * @param attr_handle
 * @param ctxt
 * @param arg
 * @return
 */
static int client_done(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    char done[] = {0x00,0x00};
    os_mbuf_append(ctxt->om, done, 2);
    currentlyReading = END;
    return 0;
}

//===============================================GATT SERVICE DEFINITION==============================================//

static const struct ble_gatt_svc_def gatt_svcs[] = {
        {
                .type = BLE_GATT_SVC_TYPE_PRIMARY,
                .uuid = BLE_UUID16_DECLARE(0x180),                 // Define UUID for device type
                .characteristics = (struct ble_gatt_chr_def[])
                        {
                                {
                                        .uuid = BLE_UUID16_DECLARE(0xFEF4),           // Read UUID for the client to see where it currently is
                                        .flags = BLE_GATT_CHR_F_READ,
                                        .access_cb = device_read
                                },
                                {
                                        .uuid = BLE_UUID16_DECLARE(0xDDEE),           // Read UUID for indicating when it's done
                                        .flags = BLE_GATT_CHR_F_READ,
                                        .access_cb = client_done
                                },
                                {
                                        .uuid = BLE_UUID16_DECLARE(0xBBBB),           // Write UUID to receive data from the client
                                        .flags = BLE_GATT_CHR_F_WRITE,
                                        .access_cb = device_write
                                },
                                {0}
                        }
        },
        {0}
};

//=====================================================BLE LIFECYCLE==================================================//

// BLE event handling
static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
        // Advertise if connected
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
            if (event->connect.status != 0)
            {
                ble_app_advertise();
            }
            break;
            // Advertise again after completion of the event
        case BLE_GAP_EVENT_ADV_COMPLETE:
            ESP_LOGI("GAP", "BLE GAP EVENT");
            ble_app_advertise();
            break;
        default:
            break;
    }
    return 0;
}

// Define the BLE connection
void ble_app_advertise(void)
{
    // GAP - device name definition
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name(); // Read the BLE device name
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    // GAP - device connectivity definition
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // connectable or non-connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN; // discoverable or non-discoverable
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

// The application
void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type); // Determines the best address type automatically
    ble_app_advertise();                     // Define the BLE connection
}

// The infinite task
void host_task(void *param)
{
    nimble_port_run(); // This function will return only when nimble_port_stop() is executed
}

void startBleServer()
{
    esp_nimble_hci_and_controller_init();      // 2 - Initialize ESP controller
    nimble_port_init();                        // 3 - Initialize the host stack
    ble_svc_gap_device_name_set("BudBroker-Soil-Sensor"); // 4 - Initialize NimBLE configuration - server name
    ble_svc_gap_init();                        // 4 - Initialize NimBLE configuration - gap service
    ble_svc_gatt_init();                       // 4 - Initialize NimBLE configuration - gatt service
    ble_gatts_count_cfg(gatt_svcs);            // 4 - Initialize NimBLE configuration - config gatt services
    ble_gatts_add_svcs(gatt_svcs);             // 4 - Initialize NimBLE configuration - queues gatt services.
    ble_hs_cfg.sync_cb = ble_app_on_sync;
    nimble_port_freertos_init(host_task);      // 6 - Run the thread
}

void stopBleServer(){
    nimble_port_stop();
}