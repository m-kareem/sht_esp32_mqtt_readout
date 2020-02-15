#!/usr/bin/env python3

"""A MQTT to InfluxDB Bridge
This script receives MQTT data and saves those to InfluxDB.
"""

import re
from typing import NamedTuple, List

import paho.mqtt.client as mqtt
from influxdb import InfluxDBClient

import time,datetime
import json


INFLUXDB_ADDRESS = 'INFLUXDB_server_address'
INFLUXDB_USER = 'admin'
INFLUXDB_PASSWORD = 'password'
INFLUXDB_DATABASE = 'database_name'

MQTT_ADDRESS = 'mqtt_server_address'
MQTT_USER = 'mqttuser'
MQTT_PASSWORD = 'mqttpassword'
MQTT_TOPIC = 'YorkLab/+/measurements/+'
MQTT_REGEX = 'YorkLab/([^/]+)/measurements/([^/]+)'
MQTT_CLIENT_ID = 'MQTTInfluxDBBridge'
noDataValue = -999
n_chan=8

influxdb_client = InfluxDBClient(INFLUXDB_ADDRESS, 8086, INFLUXDB_USER, INFLUXDB_PASSWORD, None)


class SensorData(NamedTuple):
    location: str
    device: str
    sensorType: str
    measurement: str
    value: str
    validity: str


def check_validity(value_str):
    validity = ['false'] * len(value_str)
    for i in range(len(value_str)):
        if(float(value_str[i])!=noDataValue):
            validity[i]='true'
    return validity


def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server."""
    #print('Connected with result code ' + str(rc))
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    """The callback for when a PUBLISH message is received from the server."""
    print(msg.topic + ' ' + str(msg.payload))
    #print(str(msg.payload))
    #x=msg.payload
    #y = json.loads(x)
    #print(y["temp"][0])
    sensor_data = _parse_mqtt_message(msg.topic, msg.payload.decode('utf-8'))
    print(' ---------- ')

    if sensor_data is not None:
        print('Sending sensor data to influxDB')
        _send_sensor_data_to_influxdb(sensor_data)


def _parse_mqtt_message(topic, payload):
    match = re.match(MQTT_REGEX, topic)
    if match:
        match = re.findall(r'[\w\.-]+',topic)
        location = match[0]
        #print('location= '+location)
        device = match[1]
        #print('device= '+device)
        measurement = match[3]
        #print('measurement= '+measurement)
        SensorReading=json.loads(payload)
        #print('SensorReading= '+SensorReading[measurement])
        validity=check_validity(SensorReading[measurement])
        print(validity)
        return SensorData(location, device, SensorReading["sensorType"], measurement, SensorReading[measurement],validity)
    else:
        return None


def _send_sensor_data_to_influxdb(sensor_data):
    if(len(sensor_data.validity)==8):
        print("multi-sensor device")
        json_body = [
            {
                'measurement': sensor_data.measurement,
                'tags': {
                    'location': sensor_data.location,
                    'device': sensor_data.device,
                    'sensor': sensor_data.sensorType,
                    'validity_1': sensor_data.validity[0],
                    'validity_2': sensor_data.validity[1],
                    'validity_3': sensor_data.validity[2],
                    'validity_4': sensor_data.validity[3],
                    'validity_5': sensor_data.validity[4],
                    'validity_6': sensor_data.validity[5],
                    'validity_7': sensor_data.validity[6],
                    'validity_8': sensor_data.validity[7]
                    },
                    'time': datetime.datetime.utcnow().strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3] + 'Z',
                    'fields': {
                        'value_1': sensor_data.value[0],
                        'value_2': sensor_data.value[1],
                        'value_3': sensor_data.value[2],
                        'value_4': sensor_data.value[3],
                        'value_5': sensor_data.value[4],
                        'value_6': sensor_data.value[5],
                        'value_7': sensor_data.value[6],
                        'value_8': sensor_data.value[7]
                    }
                }
            ]
    else:
        print("single-sensor device")
        json_body = [
                {
                    'measurement': sensor_data.measurement,
                    'tags': {
                        'location': sensor_data.location,
                        'device': sensor_data.device,
                        'sensor': sensor_data.sensorType,
                        'validity': sensor_data.validity[0],
                        },
                        'time': datetime.datetime.utcnow().strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3] + 'Z',
                        'fields': {
                            'value': sensor_data.value[0]
                        }
                    }
                ]

    influxdb_client.write_points(json_body)



def _init_influxdb_database():
    databases = influxdb_client.get_list_database()
    if len(list(filter(lambda x: x['name'] == INFLUXDB_DATABASE, databases))) == 0:
        influxdb_client.create_database(INFLUXDB_DATABASE)
    influxdb_client.switch_database(INFLUXDB_DATABASE)



def main():
    _init_influxdb_database()

    mqtt_client = mqtt.Client(MQTT_CLIENT_ID)
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, 1883)
    mqtt_client.loop_forever()


if __name__ == '__main__':
    print('MQTT to InfluxDB bridge')
    main()
