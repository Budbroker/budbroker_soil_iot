# Soil Grow monitor

Connect to the device via bluetooth on BudBroker grow app and pair with a grow record. The device will periodically 
measure temp, humidity, lux, soil moisture and pressure, it will calculate the VPD and dew point and post the values to the server.

The user will be able to track the grow details from the app and get notifications if the measured details are outside of the limits.

Built on an ESP32 using [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/).

## Sensors

* BME280 sensor for temp, pressure and humidity (code adapted from [utkumaden's BMX repo](https://github.com/utkumaden/esp-idf-bmx280))
* MAX44009 (Ambient light)
* Capacitive Soil moisture sensor
* ??? (looking at integrating a CO2 sensor too)

## BLE Protocol

### Data Byte table
| Data          | BYTE   |
|---------------|--------|
| DONE          | 0x00   |
| END           | 0x01   |
| USER_TOKEN    | 0x02   |
| REFRESH_TOKEN | 0x03   |
| SERVER_HOST   | 0x04   |
| GROW_ID       | 0x05   |
| WIFI_SSD      | 0x06   |
| WIFI_PASSWORD | 0x07   |
| SOIL_AIR      | 0x08   |
| SOIL_WATER    | 0x09   |
| ERROR         | 0x99   |

### TODO. Still a work in progress.
Figuring out ESP-IDF and C as I build this project.