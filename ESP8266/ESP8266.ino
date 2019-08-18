/************************* Including Libraries *********************************/
//Libraries nodig voor de sketch
#include <FS.h>
#include <DoubleResetDetector.h>
#include "bsec.h"
#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "LED.h"
#include <ArduinoJson.h>
#include "Display.h"
/************************* Defining Variables **********************************/
//Globale variabelen
// Number of seconds after reset during which a 
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 5
// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


#define def_temperature_feed "/sensors/temperature/1"
#define def_humidity_feed "/sensors/humidity/1"
#define def_pressure_feed "/sensors/pressure/1"
#define def_iaq_feed "/sensors/iaq/1"
#define def_accuracy_feed "/sensors/accuracy/1"
#define def_uptime_feed "/sensors/uptime/1"
#define def_memory_feed "/sensors/memory/1"
#define def_rssi_feed "/sensors/rssi/1"
#define def_restart_feed "/sensors/restart/1"
#define def_led_feed "/sensors/led/1"

char temperature[40] = def_temperature_feed;
const char * temperature_feed = &temperature[0];

char humidity[40] = def_humidity_feed;
const char * humidity_feed = &humidity[0];

char pressure[40] = def_pressure_feed;
const char * pressure_feed = &pressure[0];

char iaq[40] = def_iaq_feed;
const char * iaq_feed = &iaq[0];

char accuracy[40] = def_accuracy_feed;
const char * accuracy_feed = &accuracy[0];

char uptime[40] = def_uptime_feed;
const char * uptime_feed = &uptime[0];

char memory[40] = def_memory_feed;
const char * memory_feed = &memory[0];

char rssi[40] = def_rssi_feed;
const char * rssi_feed = &rssi[0];

char restart[40] = def_restart_feed;
const char * restart_feed = &restart[0];

char led[40] = def_led_feed;
const char * led_feed = &led[0];


#define AIO_SERVER      "pwsvps.ddns.net"
#define AIO_SERVERPORT  1883                  // 8883 for MQTTS
char mqtt_server[40] = "pwsvps.ddns.net";
const char * mqtt_server_ptr = &mqtt_server[0];
uint16_t mqtt_port = 1883;

/************************* Defining helper functions *********************************/
//Functies voor het checken van werking van Sensor
void checkIaqSensorStatus(void);
void errLeds(void);
/************************* Creating Objects for hardware components *********************************/
//Maakt objecten voor alle hardware
WiFiClientSecure client;
Bsec iaqSensor;
LED strip = LED(12,5);
Display display = Display();
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);
/************************* Defining global variables *********************************/
//Globale variabelen
String output;
String error;
//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
/************************* Defining MQTT server and topics *********************************/
//Maakt MQTT objecten
Adafruit_MQTT_Client mqtt(&client, mqtt_server_ptr, mqtt_port);
Adafruit_MQTT_Publish temperature_topic = Adafruit_MQTT_Publish(&mqtt, temperature_feed);
Adafruit_MQTT_Publish pressure_topic = Adafruit_MQTT_Publish(&mqtt, pressure_feed);
Adafruit_MQTT_Publish humidity_topic = Adafruit_MQTT_Publish(&mqtt, humidity_feed);
Adafruit_MQTT_Publish iaq_topic = Adafruit_MQTT_Publish(&mqtt, iaq_feed);
Adafruit_MQTT_Publish accuracy_topic = Adafruit_MQTT_Publish(&mqtt, accuracy_feed);
Adafruit_MQTT_Publish uptime_topic = Adafruit_MQTT_Publish(&mqtt, uptime_feed);
Adafruit_MQTT_Publish memory_topic = Adafruit_MQTT_Publish(&mqtt, memory_feed);
Adafruit_MQTT_Publish rssi_topic = Adafruit_MQTT_Publish(&mqtt, rssi_feed);
Adafruit_MQTT_Subscribe restart_topic = Adafruit_MQTT_Subscribe(&mqtt, restart_feed);
Adafruit_MQTT_Subscribe led_topic = Adafruit_MQTT_Subscribe(&mqtt, led_feed);
/************************* Defining bitmap of icons *********************************/
//Bitmap arrays voor alle iconen
const unsigned char PROGMEM _warning[] = {
  0x0,0x1,0x80,0x0,
  0x0,0x1,0x80,0x0,
  0x0,0x3,0xc0,0x0,
  0x0,0x3,0xc0,0x0,
  0x0,0x7,0xe0,0x0,
  0x0,0x7,0xe0,0x0,
  0x0,0xe,0x70,0x0,
  0x0,0xe,0x70,0x0,
  0x0,0x1c,0x38,0x0,
  0x0,0x1c,0x38,0x0,
  0x0,0x38,0x1c,0x0,
  0x0,0x39,0x9c,0x0,
  0x0,0x71,0x8e,0x0,
  0x0,0x71,0x8e,0x0,
  0x0,0xe1,0x87,0x0,
  0x0,0xe1,0x87,0x0,
  0x1,0xc1,0x83,0x80,
  0x3,0xc1,0x83,0xc0,
  0x3,0x81,0x81,0xc0,
  0x7,0x1,0x80,0xe0,
  0x7,0x1,0x80,0xe0,
  0xe,0x1,0x80,0x70,
  0xe,0x0,0x0,0x70,
  0x1c,0x1,0x80,0x38,
  0x1c,0x3,0xc0,0x38,
  0x38,0x3,0xc0,0x1c,
  0x38,0x1,0x80,0x1c,
  0x70,0x0,0x0,0xe,
  0x70,0x0,0x0,0xe,
  0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,
  0x7f,0xff,0xff,0xfe
};
const unsigned char PROGMEM _loading1[] = {
  0x0,0x1,0x80,0x0,
  0x0,0x1,0x80,0x0,
  0x0,0x1,0x80,0x0,
  0x1,0x81,0x81,0x80,
  0x3,0x81,0x81,0xc0,
  0x1,0xc1,0x83,0x80,
  0x1,0xe1,0x87,0x80,
  0x0,0xe1,0x87,0x0,
  0x0,0x71,0x8e,0x0,
  0x0,0x30,0xc,0x0,
  0x20,0x0,0x0,0x4,
  0x7c,0x0,0x0,0x3e,
  0x7f,0x80,0x1,0xfe,
  0xf,0x80,0x1,0xf0,
  0x1,0x80,0x1,0x80,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x1,0x80,0x1,0x80,
  0xf,0x80,0x1,0xf0,
  0x7f,0x80,0x1,0xfe,
  0x7c,0x0,0x0,0x3e,
  0x60,0x0,0x0,0x4,
  0x0,0x30,0xc,0x0,
  0x0,0x71,0x8e,0x0,
  0x0,0xe1,0x87,0x0,
  0x1,0xe1,0x87,0x80,
  0x1,0xc1,0x83,0x80,
  0x3,0x81,0x81,0xc0,
  0x1,0x81,0x81,0x80,
  0x0,0x1,0x80,0x0,
  0x0,0x1,0x80,0x0,
  0x0,0x1,0x80,0x0
};
const unsigned char PROGMEM _loading[] = {
  0x0,0x0,0x0,0x0,
  0x0,0x18,0x18,0x0,
  0x0,0x38,0x1c,0x0,
  0x0,0x18,0x18,0x0,
  0x0,0x1c,0x38,0x0,
  0x0,0x1c,0x38,0x0,
  0x8,0xc,0x30,0x10,
  0x1e,0xe,0x70,0x78,
  0x1f,0xe,0x70,0xf8,
  0x7,0x80,0x1,0xe0,
  0x3,0xc0,0x3,0xc0,
  0x0,0xc0,0x3,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0xff,0x80,0x1,0xff,
  0xff,0x80,0x1,0xff,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0xc0,0x3,0x0,
  0x3,0xc0,0x3,0xc0,
  0x7,0x80,0x1,0xe0,
  0x1f,0xe,0x70,0xf8,
  0x1e,0xe,0x70,0x78,
  0x8,0xc,0x30,0x10,
  0x0,0x1c,0x38,0x0,
  0x0,0x1c,0x38,0x0,
  0x0,0x18,0x18,0x0,
  0x0,0x38,0x1c,0x0,
  0x0,0x18,0x18,0x0,
  0x0,0x0,0x0,0x0
};
const unsigned char PROGMEM _humidity[]  = {
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x1,0x80,0x0,
  0x0,0x3,0xc0,0x0,
  0x0,0x3,0xc0,0x0,
  0x0,0x7,0xe0,0x0,
  0x0,0xd,0xf0,0x0,
  0x0,0x9,0xf0,0x0,
  0x0,0x1b,0xf8,0x0,
  0x0,0x13,0xf8,0x0,
  0x0,0x33,0xfc,0x0,
  0x0,0x23,0xfc,0x0,
  0x0,0x27,0xfc,0x0,
  0x0,0x67,0xfe,0x0,
  0x0,0x67,0xfe,0x0,
  0x0,0x67,0xfe,0x0,
  0x0,0x27,0xfc,0x0,
  0x0,0x33,0xfc,0x0,
  0x0,0x1b,0xf8,0x0,
  0x0,0xf,0xf0,0x0,
  0x0,0x3,0xc0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0
};  
const unsigned char PROGMEM _temperature[] = {
  0x0,0x1,0x80,0x0,
  0x0,0x3,0xc0,0x0,
  0x0,0x3,0xc0,0x0,
  0x0,0x3,0xc0,0x0,
  0x0,0x3,0xc0,0x0,
  0x0,0x3,0xc0,0x0,
  0x0,0x3,0xc0,0x0,
  0x0,0x2,0x40,0x0,
  0x0,0x2,0x40,0x0,
  0x0,0x2,0x40,0x0,
  0x0,0x2,0x40,0x0,
  0x0,0x2,0x40,0x0,
  0x0,0x2,0x40,0x0,
  0x0,0x2,0x40,0x0,
  0x0,0x2,0x40,0x0,
  0x0,0x2,0x40,0x0,
  0x0,0x2,0x40,0x0,
  0x0,0x2,0x40,0x0,
  0x0,0x2,0x40,0x0,
  0x0,0x6,0x60,0x0,
  0x0,0xe,0x70,0x0,
  0x0,0x1c,0x38,0x0,
  0x0,0x38,0x1c,0x0,
  0x0,0x30,0xc,0x0,
  0x0,0x30,0xc,0x0,
  0x0,0x30,0xc,0x0,
  0x0,0x30,0xc,0x0,
  0x0,0x30,0xc,0x0,
  0x0,0x38,0x1c,0x0,
  0x0,0x1e,0x78,0x0,
  0x0,0xf,0xf0,0x0,
  0x0,0x3,0xc0,0x0
};
const unsigned char PROGMEM _gas[] = {
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x1e,0x0,0x0,
  0x0,0x41,0x80,0x0,
  0x0,0x80,0x40,0x0,
  0x1,0x0,0x40,0x0,
  0x2,0x0,0x3c,0x0,
  0x2,0x0,0x1,0x0,
  0x2,0x0,0x0,0x80,
  0x2,0x0,0x0,0x40,
  0xc,0x0,0x0,0x30,
  0x30,0x0,0x0,0xc,
  0x20,0x0,0x0,0x4,
  0x40,0x0,0x0,0x2,
  0x80,0x0,0x0,0x1,
  0x80,0x0,0x0,0x1,
  0x80,0x0,0x0,0x1,
  0x80,0x0,0x0,0x1,
  0x80,0x0,0x0,0x1,
  0x40,0x0,0x0,0x2,
  0x40,0x0,0x0,0x2,
  0x20,0x0,0x0,0x4,
  0x18,0x0,0x0,0x18,
  0x7,0xff,0xff,0xe0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0
};
/************************* Setup Function *********************************/
//Functie die draait wanneer ESP opstart
void setup(void)
{
  Serial.begin(115200); //Begint seriële poort
  display.Begin();
  display.StartLoading();
  display.Load("BSEC");
  iaqSensor.begin(BME680_I2C_ADDR_PRIMARY, Wire);
  display.Load("LED");
  strip.Begin();
  display.Load("Wifi");
  setup_wifi();
  display.Load("Connected");
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);
  checkIaqSensorStatus();
  bsec_virtual_sensor_t sensorList[10] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };
  display.Load("Sensor");
  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  display.Load("MQTT");
  mqtt.subscribe(&restart_topic);
  mqtt.subscribe(&led_topic);
  MQTT_connect();
  checkIaqSensorStatus();
  display.Load("Ready!");
  display.StopLoading();
  strip.ShowRainbow();
  display.StartSlides();
}
/************************* loop Function *********************************/
void loop(void){
  unsigned long time_trigger = millis();
  if (iaqSensor.run()) { // If new data is available
    temperature_topic.publish(iaqSensor.temperature);
    humidity_topic.publish(iaqSensor.humidity);
    pressure_topic.publish(iaqSensor.pressure);
    accuracy_topic.publish(iaqSensor.iaqAccuracy);
    iaq_topic.publish(iaqSensor.iaqEstimate);
    memory_topic.publish(ESP.getFreeHeap());
    rssi_topic.publish(WiFi.RSSI());
    unsigned int uptime = millis()/1000;
    uptime_topic.publish(uptime);
    strip.ShowIAQ(iaqSensor.iaqEstimate);
    display.NextSlide(iaqSensor);
  } else {
    checkIaqSensorStatus();
    MQTT_connect();
    Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &restart_topic) {
      Serial.println((char*)restart_topic.lastread);
      Serial.println("Restarting.....");
      ESP.restart();
    }
    if (subscription == &led_topic) {
    uint8_t led_status = atoi((char*)led_topic.lastread);
    uint8_t led_on = 1;
    if(led_status == led_on){
      strip.EnableLED(iaqSensor);
    }else{
      strip.DisableLED();
    }
    drd.loop();
  }
  }
  }

}
void setup_wifi(void){
  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument doc(1024);
        auto error = deserializeJson(doc, buf.get());
        serializeJson(doc, Serial);
        JsonObject json = doc.to<JsonObject>();
        if (!error) {
          Serial.println("\nparsed json");

          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(temperature, json["temperature_topic"]);
          strcpy(humidity, json["humidity_topic"]);
          strcpy(pressure, json["pressure_topic"]);
          strcpy(iaq, json["iaq_topic"]);
          strcpy(accuracy, json["accuracy_topic"]);
          strcpy(uptime, json["uptime_topic"]);
          strcpy(memory, json["memory_topic"]);
          strcpy(rssi, json["rssi_topic"]);
          strcpy(restart, json["restart_topic"]);
          strcpy(led, json["led_topic"]);
          }
        } else {
          Serial.println("failed to load json config");
        }
      }
    } else {
    Serial.println("failed to mount FS");
  }
  //end read
  Serial.println(mqtt_server);


  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_temperature_topic("temperature_topic", "temperature_topic", temperature, 40);
  WiFiManagerParameter custom_humidity_topic("humidity_topic", "humidity_topic", humidity, 40);
  WiFiManagerParameter custom_pressure_topic("pressure_topic", "pressure_topic", pressure, 40);
  WiFiManagerParameter custom_iaq_topic("iaq_topic", "iaq_topic", iaq, 40);
  WiFiManagerParameter custom_accuracy_topic("accuracy_topic", "accuracy_topic", accuracy, 40);
  WiFiManagerParameter custom_uptime_topic("uptime_topic", "uptime_topic", uptime, 40);
  WiFiManagerParameter custom_memory_topic("memory_topic", "memory_topic", memory, 40);
  WiFiManagerParameter custom_rssi_topic("rssi_topic", "rssi_topic", rssi, 40);
  WiFiManagerParameter custom_restart_topic("restart_topic", "restart_topic", restart, 40);
  WiFiManagerParameter custom_led_topic("led_topic", "led_topic", led, 40); 
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  //add all your parameters here
  


  //reset settings - for testing
  //wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  
  if (drd.detectDoubleReset()) {
    Serial.println("Double Reset Detected");
    display.Load("Wifi\nSetup");
    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_temperature_topic);
    wifiManager.addParameter(&custom_humidity_topic);
    wifiManager.addParameter(&custom_pressure_topic);
    wifiManager.addParameter(&custom_iaq_topic);
    wifiManager.addParameter(&custom_accuracy_topic);
    wifiManager.addParameter(&custom_uptime_topic);
    wifiManager.addParameter(&custom_memory_topic);
    wifiManager.addParameter(&custom_rssi_topic);
    wifiManager.addParameter(&custom_restart_topic);
    wifiManager.addParameter(&custom_led_topic);
    wifiManager.startConfigPortal("ESP-AIR");
    delay(3000);
    ESP.reset();
    delay(5000);
  } else {
      Serial.println("No Double Reset Detected");
      WiFi.mode(WIFI_STA);
      if (!wifiManager.autoConnect("ESP-AIR")) {
      display.Load("Can't\nconnect");
      }
  }
  

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(temperature, custom_temperature_topic.getValue());
  strcpy(humidity, custom_humidity_topic.getValue());
  strcpy(pressure, custom_pressure_topic.getValue());
  strcpy(iaq, custom_iaq_topic.getValue());
  strcpy(accuracy, custom_accuracy_topic.getValue());
  strcpy(uptime, custom_uptime_topic.getValue());
  strcpy(restart, custom_restart_topic.getValue());
  strcpy(led, custom_led_topic.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();
    json["mqtt_server"] = mqtt_server;
    json["temperature_topic"] = temperature;
    json["humidity_topic"] = humidity;
    json["pressure_topic"] = pressure;
    json["iaq_topic"] = iaq;
    json["accuracy_topic"] = accuracy;
    json["uptime_topic"] = uptime;
    json["memory_topic"] = memory;
    json["rssi_topic"] = rssi;
    json["restart_topic"] = restart;
    json["led_topic"] = led;
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    serializeJsonPretty(doc, Serial);
    serializeJson(doc, configFile);
    configFile.close();
    //end save
    
}
}
void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 5;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 1 second...");
    mqtt.disconnect();
    delay(500);  // wait a half second
    retries--;
    if (retries == 0) {
    }
  }
  
  Serial.println("MQTT Connected!");
}
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      display.ShowError("BSEC: " + String(iaqSensor.status));
      for (;;)
        
        errLeds(); /* Halt in case of failure */
        
    } else {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme680Status != BME680_OK) {
    if (iaqSensor.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      display.ShowError("BME: " + String(iaqSensor.bme680Status));
      for (;;)
        
        errLeds(); /* Halt in case of failure */
        
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

void errLeds(void)
{
  display.StopSlides();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
