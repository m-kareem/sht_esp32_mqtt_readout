# SHT_mqtt_local

This is an arduino code written for ESP32 micro-controller, which reads the humidity and temperature from an SHT3x sensor and transmit the data to MQTT server.

- The code can handle both regular and enterprise wifi connections.

- One the server side, a python script is listening to the mqtt channels, pars the messages and write the data to influxDB.

There are two config files:
- single sensor: the sensor is directly connected to the ESP32
- multi sensors: the sensors are connected through I2C multiplexor to the ESP32
