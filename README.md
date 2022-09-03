# Soil Grow monitor

Connect to the device via bluetooth on BudBroker grow app and pair with a grow record. The device will periodically 
measure temp, humidity, lux, soil moisture and pressure, it will calculate the VPD and dew point and post the values to the server.

The user will be able to track the grow details from the app and get notifications if the measured details are outside of the limits.

Built on an ESP32 using ESP-IDF.

## Sensors

* DHT11 (temperature/humidity)
* MAX44009 (Ambient light)
* Soil moisture sensor
* Barometric Pressure sensor
* ??? (looking at integrating a CO2 sensor too)

### TODO. Still a work in progress.