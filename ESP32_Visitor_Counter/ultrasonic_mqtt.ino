#include <WiFi.h>
#include <PubSubClient.h>

// ======================================================
//                  ULTRASONIC PINS
// ======================================================

// Sensor 1
#define TRIG1 5
#define ECHO1 18

// Sensor 2
#define TRIG2 14
#define ECHO2 27

// ======================================================
//                  WIFI CREDENTIALS
// ======================================================

const char* ssid = "ADI";
const char* password = "adi321!!";

// ======================================================
//                  MQTT SETTINGS
// ======================================================

const char* mqtt_server = "broker.emqx.io";
const int mqttPort = 1883;

const char* mqtt_User = "admin";
const char* mqtt_Password = "public";

// ======================================================
//                  MQTT TOPICS
// ======================================================

#define VISITOR_TOPIC "hospital/ward1/visitor"

#define COUNT_TOPIC "hospital/ward1/visitor_count"

// ======================================================
//                  MQTT OBJECTS
// ======================================================

WiFiClient espClient;
PubSubClient client(espClient);

// ======================================================
//                  VARIABLES
// ======================================================

float distance1 = 0;
float distance2 = 0;

bool sensor1Triggered = false;
bool sensor2Triggered = false;

unsigned long sensor1Time = 0;
unsigned long sensor2Time = 0;

// Stable detection counters
int stableCount1 = 0;
int stableCount2 = 0;

// Visitor count
int visitorCount = 0;

// Settings
const int DETECTION_DISTANCE = 40;
const int REQUIRED_STABLE_COUNT = 3;
const unsigned long TIMEOUT = 3000;

// ======================================================
//                  WIFI CONNECTION
// ======================================================

void setup_wifi() {

  Serial.println("Connecting to WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ======================================================
//                  MQTT RECONNECT
// ======================================================

void reconnect() {

  while (!client.connected()) {

    Serial.println("Connecting to MQTT Broker...");

    if (client.connect("VisitorCounterESP32",
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
//              DISTANCE MEASUREMENT
// ======================================================

float getDistance(int trigPin, int echoPin) {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  // No echo received
  if (duration == 0) {

    return 999;
  }

  float distance = duration * 0.0343 / 2;

  return distance;
}

// ======================================================
//                PUBLISH VISITOR COUNT
// ======================================================

void publishVisitorCount() {

  String countStr = String(visitorCount);

  client.publish(COUNT_TOPIC,
                 countStr.c_str());

  Serial.print("Visitor Count: ");
  Serial.println(visitorCount);
}

// ======================================================
//                        SETUP
// ======================================================

void setup() {

  Serial.begin(115200);

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  setup_wifi();

  client.setServer(mqtt_server, mqttPort);

  Serial.println("Visitor Counter Ready");
}

// ======================================================
//                        LOOP
// ======================================================

void loop() {

  // MQTT connection
  if (!client.connected()) {

    reconnect();
  }

  client.loop();

  // ==================================================
  //              READ SENSOR 1
  // ==================================================

  distance1 = getDistance(TRIG1, ECHO1);

  delay(20);

  // ==================================================
  //              READ SENSOR 2
  // ==================================================

  distance2 = getDistance(TRIG2, ECHO2);

  // ==================================================
  //                SERIAL MONITOR
  // ==================================================

  Serial.print("Sensor1: ");
  Serial.print(distance1);

  Serial.print(" cm  |  Sensor2: ");
  Serial.print(distance2);

  Serial.println(" cm");

  // ==================================================
  //          SENSOR 1 STABLE DETECTION
  // ==================================================

  if (distance1 > 2 &&
      distance1 <= DETECTION_DISTANCE) {

    stableCount1++;

    if (stableCount1 >= REQUIRED_STABLE_COUNT &&
        !sensor1Triggered) {

      sensor1Triggered = true;

      sensor1Time = millis();

      Serial.println("Sensor 1 Triggered");

      stableCount1 = 0;
    }

  } else {

    stableCount1 = 0;
  }

  // ==================================================
  //          SENSOR 2 STABLE DETECTION
  // ==================================================

  if (distance2 > 2 &&
      distance2 <= DETECTION_DISTANCE) {

    stableCount2++;

    if (stableCount2 >= REQUIRED_STABLE_COUNT &&
        !sensor2Triggered) {

      sensor2Triggered = true;

      sensor2Time = millis();

      Serial.println("Sensor 2 Triggered");

      stableCount2 = 0;
    }

  } else {

    stableCount2 = 0;
  }

  // ==================================================
  //                ENTRY DETECTION
  // ==================================================

  if (sensor1Triggered &&
      sensor2Triggered &&
      sensor1Time < sensor2Time) {

    Serial.println("PERSON ENTERED");

    // Publish event
    client.publish(VISITOR_TOPIC,
                   "PERSON_ENTERED");

    // Increase count
    visitorCount++;

    // Publish updated count
    publishVisitorCount();

    // Reset
    sensor1Triggered = false;
    sensor2Triggered = false;

    delay(1000);
  }

  // ==================================================
  //                EXIT DETECTION
  // ==================================================

  else if (sensor1Triggered &&
           sensor2Triggered &&
           sensor2Time < sensor1Time) {

    Serial.println("PERSON EXITED");

    // Publish event
    client.publish(VISITOR_TOPIC,
                   "PERSON_EXITED");

    // Prevent negative count
    if (visitorCount > 0) {

      visitorCount--;
    }

    // Publish updated count
    publishVisitorCount();

    // Reset
    sensor1Triggered = false;
    sensor2Triggered = false;

    delay(1000);
  }

  // ==================================================
  //                  TIMEOUT RESET
  // ==================================================

  if (sensor1Triggered &&
      (millis() - sensor1Time > TIMEOUT)) {

    sensor1Triggered = false;

    Serial.println("Sensor1 Timeout Reset");
  }

  if (sensor2Triggered &&
      (millis() - sensor2Time > TIMEOUT)) {

    sensor2Triggered = false;

    Serial.println("Sensor2 Timeout Reset");
  }

  delay(20);
}