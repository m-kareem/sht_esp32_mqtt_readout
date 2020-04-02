# SHT sensor readout cia ESP32 through MQTT

This is an arduino code written for ESP32 micro-controller, which reads the humidity and temperature from an SHT3x sensor and transmit the data to MQTT server.

- The code can handle both regular and enterprise wifi connections.

- One the server side, a python script (main_v2.py needs to be running in background) is listening to the mqtt channels, pars the messages and write the data to influxDB.

There are two config files:
- single sensor: the sensor is directly connected to the ESP32
- multi sensors: the sensors are connected through I2C multiplexor to the ESP32

I have also designed two PCBs for a cleaner setup (you can contact me if you need more details on how to order low cost PCB). Files are provided.

In addition, you can accomodate the module in a nice and simple 3D printed box. stp file is provided.
