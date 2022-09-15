#ifndef flash
#define flash

#define USER_ACCESS_TOKEN "token"
#define USER_REFRESH_TOKEN "refreshToken"
#define SERVER_HOST "server_host"
#define GROW_ID "growId"
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "wifiPass"
#define SOIL_MONITOR_AIR "soilMonitorAir"
#define SOIL_MONITOR_WATER "soilMonitorWater"

#include <stdio.h>
#include <stdint.h>

void init_storage();
char* get_stored_value(char *key);
void set_storage_value(char *key, char *val);
void erase_value(char *key);
int get_stored_int_value(char *key);
void set_int_storage_value(char *key, int32_t val);

#endif