# SHT sensor readout cia ESP32 through MQTT

This is an arduino code written for ESP32 micro-controller, which reads the humidity and temperature from an SHT3x sensor and transmit the data to MQTT server.

- The code can handle both regular and enterprise wifi connections.

- One the server side, a python script (main_v2.py needs to be running in background) is listening to the mqtt channels, pars the messages and write the data to influxDB.

There are two config files:
- single sensor: the sensor is directly connected to the ESP32
- multi sensors: the sensors are connected through I2C multiplexor to the ESP32

I have also designed two PCBs for a cleaner assembly. Files are provided in PCB folder.
You can design your own PCBs via Fritzing software and order it for low cost via aisler.net, and get it shipped to you. They are doing really good job. 

In addition, you can accomodate the module in a nice and simple 3D printed box. stl file is provided.

# Required hardwares:
- EST32 microcontroller
- seeedstudio SHT35 Temp&Humi Sensor (you can use any other compatible alternative)
- Adafruit TCA9548A 1 to 8 I2C Multiplexer (for multi-sensor setup)

