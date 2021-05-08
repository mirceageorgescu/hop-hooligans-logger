#include <ESP8266WiFi.h>
#include <DHT.h>

const int httpsPort = 443;
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";

// Replace with your SSID and Password
const char* ssid     = "Pretty fly for a Wi-Fi";
const char* password = "arbustului29";

// Replace with your google drive data
const char* host = "script.google.com";
String GAS_ID = "AKfycbx3iqGy1P-dqe3yM0ApEYBJddWE2eCKxukfqM3n7T4GPzNJTD7PJWFwBE4PT895ZMOw";  // Replace with your GAS service id

// Time to sleep
uint64_t uS_TO_S_FACTOR = 1000000;  // Conversion factor for micro seconds to seconds

// sleep for 1h = 3600 seconds
uint64_t TIME_TO_SLEEP = 60; //seconds

// DHT config.
#define DHTPIN 2 // Pin which is connected to the DHT sensor.
#define DHTTYPE DHT11 // DHT 11

ADC_MODE(ADC_VCC);
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

float batterylevel;
float humidity = 0.0f;
float temperature = 0.0f; // Values read from sensor
int dhtCount = 0;
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(115200);
  delay(2000);

  batterylevel = ESP.getVcc() / 1000.00;

  dht.begin();
  
  initWifi();

  // Wait 2s for DHT22, this is valid for mine
  // SI7021 sensor and not DHT22 anymore. Si7021 is much faster (doesn’t need 2s) with the same (or better) accuracy
  // Check my nect project http://homecircuits.eu/blog/battery-wifi-iot-temp-hum-soil-moisture-sensors/
  while (millis() < 2100)
    delay(10);
  gettemperature();

  //post data to google sheets
  sendData(temperature, humidity, batterylevel);

  // Deep sleep mode
  ESP.deepSleep(TIME_TO_SLEEP * uS_TO_S_FACTOR); 
}

void loop() {
  // sleeping so wont get here 
}

// Establish a Wi-Fi connection with your router
void initWifi() {
  WiFi.begin(ssid, password);  

  int timeout = 10 * 4; // 10 seconds
  while(WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(250);
  }
}

// took a while to find the fasted way to read DHT22
void gettemperature() {
  bool dhtOK = false;

  humidity = dht.readHumidity();
  dhtCount++;
  while (dhtOK != true) {
    dhtOK = true; // let's assume the reading is OK
    if (isnan(humidity) || isnan(temperature))
      dhtOK = false;
    if ((dhtOK == true) && (humidity == 0.0))
      dhtOK = false;
    if (dhtOK == false)
    { delay(200);
      humidity = dht.readHumidity();          // Read humidity (percent)
      //temperature = dht.readTemperature();     // Read temperature as *C
      dhtCount++;
    }
    if (dhtOK == true)
      temperature = dht.readTemperature();     // Read initial temp
  }
}

void sendData(float temperature, float humidity, float battery) {
  WiFiClientSecure client;
  client.setInsecure();

  int retries = 5;
  while(!!!client.connect(host, httpsPort) && (retries-- > 0)) {
    Serial.print(".");
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }
  
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + String(temperature) + "&humidity=" + String(humidity) + "&battery=" + String(battery);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");
  
  int timeout = 5 * 10; // 5 seconds             
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);
  }
  while(client.available()){
    Serial.write(client.read());
  }

  client.stop();
}