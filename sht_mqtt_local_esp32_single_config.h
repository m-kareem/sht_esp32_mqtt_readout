/* This file contains your configuration used to:
 *  1- connect to WIFI (enterprise or non-EAP)
 *  2- Set the device configuration (sensor type and multiplicity)
 *  3- Set the MQTT configuration
 */

#define TCAADDR 0x70 // I2C multiplexer address

// ============ device configuration =======================================
const bool multiSensor=false; // true if there are multiple sensors connected via I2C, false otherwise
//const char *device_location="YorkLab"; // Where the device is located?
const char *device_name="esp32_1"; // Give the device a name!
const char *sensor_type="SHT35"; // supported sensors: SHT35 , SHT21

// ============ NETWORK setting =======================================
const bool EAP_wifi=true; // change according to the WIFI type [true for enterprise networks, false for normal ]

//------ ENT Wifi setting (e.g. eduroam)-------------------
#define EAP_ANONYMOUS_IDENTITY "username@example.com"
#define EAP_IDENTITY "username@example.com"
#define EAP_PASSWORD "your_password" // hardcode your wifi password here
const char* EAP_ssid = "AirYorkPLUS"; //  network's SSID

//------ non-ENT Wifi setting-------------------
const char *ssid = "OnePlus6";
const char *password = "your_password";


// ============ MQTT setting =======================================
#define MQTT_PUBLISH_DELAY 5000
#define MQTT_CLIENT_ID "esp32_single_sht35"
const char *MQTT_SERVER = "petra.phys.yorku.ca";
const int MQTT_PORT = 1883;
const char *MQTT_USER = "mqttuser"; // NULL for no authentication
const char *MQTT_PASSWORD = "mqttpassword"; // NULL for no authentication

//-------------MQTT topics --------------------
#define MQTT_TOPIC_STATE "YorkLab/esp32_1/status"
const char *MQTT_TOPIC_HUMIDITY= "YorkLab/esp32_1/measurements/rH";
const char *MQTT_TOPIC_TEMPERATURE= "YorkLab/esp32_1/measurements/T";
