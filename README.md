# SHT_mqtt_local

This is an arduino code for ESP32 micro-controller, which reads the humidity and temperature from an SHT3x sensor and transmit the data to MQTT server.

There are two config files:
- single sensor: the sensor is directly connected to the ESP32
- multi sensors: the sensors are connected through I2C multiplexor to the ESP32
- the code can handle both regular and enterprise wifi connections
