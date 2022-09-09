#ifndef flash
#define flash

#define USER_ACCESS_TOKEN "token"
#define USER_REFRESH_TOKEN "refreshToken"
#define SERVER_HOST "server_host"
#define RECORD_ID "growId"
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "wifiPass"
#define SOIL_MONITOR_HIGH "soilMonitorHigh"
#define SOIL_MONITOR_LOW "soilMonitorLow"

void init_storage();
char* get_stored_value(char *key);
void set_storage_value(char *key, char *val);
void erase_value(char *key);

#endif