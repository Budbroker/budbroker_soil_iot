#ifndef flash
#define flash

#define USER_ACCESS_TOKEN_KEY "token"
#define USER_REFRESH_TOKEN_KEY "refreshToken"
#define SERVER_HOST_KEY "server_host"
#define GROW_ID_KEY "growId"
#define WIFI_SSID_KEY "ssid"
#define WIFI_PASSWORD_KEY "wifiPass"
#define SOIL_MONITOR_AIR_KEY "soilMonitorAir"
#define SOIL_MONITOR_WATER_KEY "soilMonitorWater"

#include <stdio.h>
#include <stdint.h>

void init_storage();
char* get_stored_value(char *key);
void set_storage_value(char *key, char *val);
void erase_value(char *key);
int get_stored_int_value(char *key);
void set_int_storage_value(char *key, int32_t val);

#endif