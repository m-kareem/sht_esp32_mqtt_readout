
/**
    Required libraries:
      - Seeed_SHT35
      - Sodaq_SHT2x
      - PubSubClient
**/

//#include "sht_mqtt_local_esp32_single_config.h" // Update this file with your configuration
#include "sht_mqtt_local_esp32_multi_config.h"

#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include "Seeed_SHT35.h"
#include <Sodaq_SHT2x.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <MQTT.h>

#define LED 2


/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SDAPIN  20
  #define SCLPIN  21
  #define RSTPIN  7
  #define SERIAL SerialUSB
#else
  #define SDAPIN  A4
  #define SCLPIN  A5
  #define RSTPIN  2
  #define SERIAL Serial
#endif

// ----------- variable definitions
int counter = 0;
int n_chan;
float rH_list[8];
float T_list[8];
const float invalidData = -999;

//---- objects
SHT35 sensor(SCLPIN);
WiFiClient espClient;
PubSubClient mqttClient(espClient);


//================== Functions ==============================
void setupNetwork() {
  //bool wifi_status = false;
  if (EAP_wifi){
   setupEAP_Wifi();
  }
  else{
    setupWifi();
  }
    
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Connected to network");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

//------------------------------
void setupWifi() {
  Serial.print("Connecting to network: ");
  Serial.println(ssid);
  WiFi.disconnect(true);
  delay(100);  // <- fixes some issues with WiFi stability
  WiFi.begin(ssid, password);
 /*
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.println("Connected to network");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  */
}

//---------------------------------------------------------------
void setupEAP_Wifi() {
  Serial.print("Connecting to network: ");
  Serial.println(EAP_ssid);
  WiFi.disconnect(true);
  delay(100);  // <- fixes some issues with WiFi stability

  WiFi.mode(WIFI_STA); //init wifi mode
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY)); 
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function

  WiFi.begin(EAP_ssid); //connect to wifi
/*
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Connected to network");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } 
  */  
}

//---------------------------------------------------------------
void mqttReconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD, MQTT_TOPIC_STATE, 1, true, "disconnected", false)) {
      Serial.println("connected");

      // Once connected, publish an announcement...
      mqttClient.publish(MQTT_TOPIC_STATE, "connected", true);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

//---------------------------------------------------------------
void mqttPublish(const char *topic, float payload) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  mqttClient.publish(topic, String(payload).c_str(), true);
}

//-----------------------------------------------------------------
void mqtt_jason_publish(String topic, String dataType, float list[])
{
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  JSONencoder["device"] = device_name;
  JSONencoder["sensorType"] = sensor_type;
  
  JsonArray& values = JSONencoder.createNestedArray(dataType);
  
  for (int i = 0; i < n_chan; i++){
      values.add(list[i]);    
    }
 
  char JSONmessageBuffer[400];
  
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("Sending message to MQTT topic..");
  Serial.println(JSONmessageBuffer);

  if (mqttClient.publish(topic.c_str(), JSONmessageBuffer) == true) {
    Serial.println("Success sending message");
  } else {
    Serial.println("Error sending message");
  }
    
}

//------------------------------------------
void SetI2CConnection(uint8_t i) {
  if (i > n_chan-1) return;                // If more than 7 out of range, do nothing
  
  Wire.beginTransmission(TCAADDR);     // All data to go to address 0x70 (The Multiplexer address)
  Wire.write(1 << i);               // write value as a single bit in an 8bit byte, MSB being I2C no. 7 LSB being I2C no. 0
  Wire.endTransmission();           // Signal sending stopped
  //Serial.printf("\nI2C channel %d is selected! \n", i);
}

//----------------------------------------------
void ReadSystem(){
  float temp, hum;
  for (int i = 0; i < n_chan; i++){
    
    if(multiSensor) SetI2CConnection(i);   // Loop through each connected I2C sensor on the I2C buses
    
    if(sensor_type=="SHT35"){
      if(NO_ERROR==sensor.read_meas_data_single_shot(HIGH_REP_WITH_STRCH, &temp, &hum)){
        T_list[i]=round(temp*100)/100;
        rH_list[i]=round(hum*100)/100;
      }
      else{T_list[i]= invalidData; rH_list[i]= invalidData;}
      
    }
    else if(sensor_type=="SHT21"){
      if(SHT2x.GetTemperature()==-273) {T_list[i]= invalidData; rH_list[i]= invalidData;}
      else{
        T_list[i]= round(SHT2x.GetTemperature()*100)/100;
        rH_list[i]= round(SHT2x.GetHumidity()*100)/100;
      }
    } 
    else{
      Serial.println("Sensor is not supported!");
      return;
      }
    //delay(50);
  }// end looping over i2c channels  
}

//----------------------------------------------
void PrintResults(){
    Serial.print("Humidity(%RH): ");
    for (int i = 0; i < n_chan; i++){
      //if(rH_list[i]== invalidData) 
        //Serial.printf("%s,  ","none");
      //else
        Serial.printf("%4.1f,  ",rH_list[i]);
    }  
    Serial.print("\n");

    Serial.print("Temperature(C): ");
    for (int i = 0; i < n_chan; i++){
      //if(T_list[i]== invalidData) 
       // Serial.printf("%s,  ","none");
      //else
        Serial.printf("%4.1f,  ",T_list[i]);
    }  
    Serial.print("\n");
}

//---------------------------------------------
void setNchan(){
  if(multiSensor) n_chan=8;
  else n_chan=1;
}

//==============================================
void setup() {
  while (!Serial);
  delay(500);
  Serial.begin(115200);
  Wire.begin(); // SDA, SCL
  //delay(100);
  pinMode(LED,OUTPUT);

  setupNetwork();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  setNchan();
}
//------------------------------
void loop() {
    u16 value=0;
    u8 data[6]={0};
    float temp,hum;
    
    if(WiFi.status() != WL_CONNECTED){
      counter++;
      digitalWrite(LED,HIGH);
      Serial.print("Retry connecting to network: ");
      Serial.println(counter);
      setupNetwork();
            
      if(counter>=10) //after 10 iteration timeout - reset board
        ESP.restart();
    }
    else{
        
      if (!mqttClient.connected()){
        mqttReconnect();
      }
      
      ReadSystem();
      //PrintResults();
      
      mqtt_jason_publish(MQTT_TOPIC_HUMIDITY, "rH", rH_list);
      mqttClient.loop();
      
      mqtt_jason_publish(MQTT_TOPIC_TEMPERATURE, "T", T_list);  
      mqttClient.loop();
      
      Serial.println("-------------");
  
      //--- LED signal on successful submission
      digitalWrite(LED,HIGH);
      delay(100);
      digitalWrite(LED,LOW); 
  }
  delay(MQTT_PUBLISH_DELAY);
}
