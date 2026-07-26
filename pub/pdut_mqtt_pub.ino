#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ======================================================
//                  Sensor Pin Configuration
// ======================================================

#define DHTPIN      4
#define DHTTYPE     DHT11

#define PIR_PIN     27
#define TOUCH_PIN   14

// ======================================================
//                  WiFi Credentials
// ======================================================

const char* ssid = "ADI";
const char* password = "adi321!!";

// ======================================================
//                  MQTT Broker Settings
// ======================================================

const char* mqtt_server   = "broker.emqx.io";
const int mqttPort        = 1883;

const char* mqtt_User     = "admin";
const char* mqtt_Password = "public";

// ======================================================
//                  Objects
// ======================================================

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

// ======================================================
//                  WiFi Connection
// ======================================================

void setup_wifi() {

  Serial.println("Connecting to WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
}

// ======================================================
//                  MQTT Reconnection
// ======================================================

void reconnect() {

  while (!client.connected()) {

    Serial.println("Connecting to MQTT Broker...");

    if (client.connect("PatientESP32",
                       mqtt_User,
                       mqtt_Password)) {

      Serial.println("MQTT Connected");

    } else {

      Serial.print("Failed, rc=");
      Serial.println(client.state());

      delay(2000);
    }
  }
}

// ======================================================
//                        Setup
// ======================================================

void setup() {

  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(TOUCH_PIN, INPUT);

  dht.begin();

  setup_wifi();

  client.setServer(mqtt_server, mqttPort);
}

// ======================================================
//                         Loop
// ======================================================

void loop() {

  if (!client.connected()) {

    reconnect();
  }

  client.loop();

  // ==================================================
  //                 DHT11 Sensor
  // ==================================================

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (!isnan(temp)) {

    char tempStr[10];

    dtostrf(temp, 1, 2, tempStr);

    client.publish("hospital/ward1/temp", tempStr);

    Serial.print("Temperature: ");
    Serial.println(tempStr);
  }

  if (!isnan(hum)) {

    char humStr[10];

    dtostrf(hum, 1, 2, humStr);

    client.publish("hospital/ward1/humidity", humStr);

    Serial.print("Humidity: ");
    Serial.println(humStr);
  }

  // ==================================================
  //                    PIR Sensor
  // ==================================================

  int pirState = digitalRead(PIR_PIN);

  if (pirState == HIGH) {

    client.publish("hospital/ward1/motion",
                   "PATIENT MOVING");

    Serial.println("Movement Detected");
  }

  // ==================================================
  //                 Touch Sensor
  // ==================================================

  int touchState = digitalRead(TOUCH_PIN);

  if (touchState == HIGH) {

    client.publish("hospital/ward1/emergency",
                   "EMERGENCY");

    Serial.println("Emergency Triggered");

    delay(1000);
  }

  delay(1000);
}