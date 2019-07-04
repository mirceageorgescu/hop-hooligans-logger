#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>

// Replace with your SSID and Password
const char* ssid     = "Hop Hooligans";
const char* password = "florinsalam";

// Replace with your unique IFTTT URL resource
const char* resource = "/trigger/camera_frigorifica/with/key/bigKZCvP-kT-Hnu8IbQax4";
//const char* resource = "/trigger/materie_prima/with/key/bigKZCvP-kT-Hnu8IbQax4";
//const char* resource = "/trigger/frigider_hamei/with/key/bigKZCvP-kT-Hnu8IbQax4";
//const char* resource = "/trigger/frigider_drojdie/with/key/bigKZCvP-kT-Hnu8IbQax4";
//const char* resource = "/trigger/ventilatie/with/key/bigKZCvP-kT-Hnu8IbQax4";

// Maker Webhooks IFTTT
const char* server = "maker.ifttt.com";

// Time to sleep
uint64_t uS_TO_S_FACTOR = 1000000;  // Conversion factor for micro seconds to seconds
// sleep for 3h = 10800 seconds
uint64_t TIME_TO_SLEEP = 3600;

ADC_MODE(ADC_VCC)  
float batterylevel;

// DHT config.
#define DHTPIN 2 // Pin which is connected to the DHT sensor.
#define DHTTYPE DHT11 // DHT 11 

DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

float humidity = 0.0f;
float temperature = 0.0f; // Values read from sensor
int dhtCount = 0;

uint32_t delayMS;
int status = WL_IDLE_STATUS;
float myTemperature = 0, myHumidity = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);

  batterylevel = ESP.getVcc() / 1000.00;

  dht.begin();
  
  initWifi();
  makeIFTTTRequest();

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


// Make an HTTP request to the IFTTT web service
void makeIFTTTRequest() {
  // Wait 2s for DHT22, this is valid for mine
  // SI7021 sensor and not DHT22 anymore. Si7021 is much faster (doesn’t need 2s) with the same (or better) accuracy
  // Check my nect project http://homecircuits.eu/blog/battery-wifi-iot-temp-hum-soil-moisture-sensors/
  while (millis() < 2100)
    delay(10);
  gettemperature();

  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }

  // Build json
  String jsonObject = String("{\"value1\":\"") + String(temperature) + "\",\"value2\":\"" + String(humidity) + "\",\"value3\":\"" + batterylevel + "\"}";
                      
  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server); 
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);
        
  int timeout = 5 * 10; // 5 seconds             
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);
  }
  while(client.available()){
    Serial.write(client.read());
  }

  client.stop(); 
}
