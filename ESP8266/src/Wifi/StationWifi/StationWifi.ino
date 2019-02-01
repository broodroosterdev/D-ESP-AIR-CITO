#include <bsec.h>
#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
//needed for wifimanager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
// Set the MQTT feeds to be used
#define temperature_feed "/sensors/temperature/1"
#define humidity_feed "/sensors/humidity/1"
#define pressure_feed "/sensors/pressure/1"
#define iaq_feed "/sensors/iaq/1"

/************************* WiFi Access Point *********************************/

WiFiClientSecure client;

/************************* MQTT Broker Setup *********************************/

#define AIO_SERVER      "pwsvps.ddns.net"
#define AIO_SERVERPORT  1883                  // 8883 for MQTTS
#define AIO_USERNAME    "mqttusername"
#define AIO_KEY         "mqttpassword"

/****************************** Feeds ***************************************/

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT);

Adafruit_MQTT_Publish temperature_topic = Adafruit_MQTT_Publish(&mqtt, temperature_feed);
Adafruit_MQTT_Publish pressure_topic = Adafruit_MQTT_Publish(&mqtt, pressure_feed);
Adafruit_MQTT_Publish humidity_topic = Adafruit_MQTT_Publish(&mqtt, humidity_feed);
Adafruit_MQTT_Publish iaq_topic = Adafruit_MQTT_Publish(&mqtt, iaq_feed);
/*************************** Sketch Code ************************************/

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);

// Create an object of the class Bsec
Bsec iaqSensor;

String output;

// Entry point for the example
void setup(void)
{
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.autoConnect();
  Serial.println("connected...jeej :)");
  MQTT_connect();
  initBME680();
}

// Function that is looped forever
void loop(void)
{
  if (iaqSensor.run()) { // If new data is available
    output = String(millis());
    output += ", " + String(iaqSensor.iaqEstimate);
    output += ", " + String(iaqSensor.iaqAccuracy);
    Serial.println(output);
    temperature_topic.publish(iaqSensor.temperature);
    humidity_topic.publish(iaqSensor.humidity);
    pressure_topic.publish(iaqSensor.pressure);
    iaq_topic.publish(iaqSensor.iaqEstimate);
    Serial.println("Data posted to MQTT");
  } else {
    checkIaqSensorStatus();
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
// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
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
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

void initBME680(void)
{
  iaqSensor.begin(BME680_I2C_ADDR_PRIMARY, Wire);
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);
  checkIaqSensorStatus();

  bsec_virtual_sensor_t sensorList[7] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  iaqSensor.updateSubscription(sensorList, 7, BSEC_SAMPLE_RATE_LP);
}

void errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
