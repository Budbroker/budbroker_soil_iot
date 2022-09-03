#ifndef flash
#define flash

#define USER_ACCESS_TOKEN "token"
#define USER_REFRESH_TOKEN "refreshToken"
#define HOST "host"
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "wifiPass"

void init_storage();
char get_stored_value(char key);
void set_storage_value(char key, char val);

#endif