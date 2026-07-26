#include <WiFi.h>
#include <PubSubClient.h>

// ======================================================
// PIN CONFIGURATION
// ======================================================

#define LED_PIN      25
#define BUZZER_PIN   26
#define BUTTON_PIN   27

// ======================================================
// WIFI CREDENTIALS
// ======================================================

const char* ssid = "ADI";
const char* password = "adi321!!";

// ======================================================
// MQTT SETTINGS
// ======================================================

const char* mqtt_server = "broker.emqx.io";
const int mqttPort = 1883;

const char* mqtt_User = "admin";
const char* mqtt_Password = "public";

// ======================================================
// MQTT TOPICS
// ======================================================

#define EMERGENCY_TOPIC  "hospital/ward1/emergency"
#define VISITOR_TOPIC    "hospital/ward1/visitor"

#define ACK_TOPIC        "hospital/ward1/acknowledged"

// ======================================================
// MQTT OBJECTS
// ======================================================

WiFiClient espClient;
PubSubClient client(espClient);

// ======================================================
// VARIABLES
// ======================================================

bool emergencyActive = false;

unsigned long previousMillis = 0;

// ======================================================
// WIFI CONNECTION
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
// VISITOR ALERT
// ======================================================

void visitorAlert() {

  Serial.println("Visitor Entered");

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);

  delay(300);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

// ======================================================
// EMERGENCY ALERT
// ======================================================

void handleEmergencyAlert() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 300) {

    previousMillis = currentMillis;

    digitalWrite(
      LED_PIN,
      !digitalRead(LED_PIN)
    );

    digitalWrite(
      BUZZER_PIN,
      !digitalRead(BUZZER_PIN)
    );
  }
}

// ======================================================
// MQTT CALLBACK
// ======================================================

void callback(
  char* topic,
  byte* payload,
  unsigned int length
) {

  String message = "";

  for (int i = 0; i < length; i++) {

    message += (char)payload[i];
  }

  Serial.print("Message Received [");
  Serial.print(topic);
  Serial.print("] : ");
  Serial.println(message);

  // ==========================================
  // VISITOR ALERT
  // ==========================================

  if (String(topic) == VISITOR_TOPIC) {

    if (message == "PERSON_ENTERED") {

      visitorAlert();
    }
  }

  // ==========================================
  // EMERGENCY ALERT
  // ==========================================

  if (String(topic) == EMERGENCY_TOPIC) {

    if (message == "EMERGENCY") {

      emergencyActive = true;

      Serial.println(
        "Emergency Alert Activated"
      );
    }
  }
}

// ======================================================
// MQTT RECONNECT
// ======================================================

void reconnect() {

  while (!client.connected()) {

    Serial.println(
      "Connecting to MQTT Broker..."
    );

    if (

      client.connect(
        "NurseESP32",
        mqtt_User,
        mqtt_Password

      )

    ) {

      Serial.println(
        "MQTT Connected"
      );

      client.subscribe(
        EMERGENCY_TOPIC
      );

      client.subscribe(
        VISITOR_TOPIC
      );

      Serial.println(
        "Subscribed to Topics"
      );

    }

    else {

      Serial.print("Failed rc = ");

      Serial.println(
        client.state()
      );

      delay(2000);
    }
  }
}

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);

  pinMode(
    LED_PIN,
    OUTPUT
  );

  pinMode(
    BUZZER_PIN,
    OUTPUT
  );

  pinMode(
    BUTTON_PIN,
    INPUT_PULLUP
  );

  digitalWrite(
    LED_PIN,
    LOW
  );

  digitalWrite(
    BUZZER_PIN,
    LOW
  );

  setup_wifi();

  client.setServer(
    mqtt_server,
    mqttPort
  );

  client.setCallback(
    callback
  );

  Serial.println(
    "Nurse Alert System Ready"
  );
}

// ======================================================
// LOOP
// ======================================================

void loop() {

  if (!client.connected()) {

    reconnect();
  }

  client.loop();

  // ==========================================
  // HANDLE EMERGENCY
  // ==========================================

  if (emergencyActive) {

    handleEmergencyAlert();
  }

  // ==========================================
  // ACKNOWLEDGEMENT BUTTON
  // ==========================================

  if (digitalRead(BUTTON_PIN) == LOW) {

    emergencyActive = false;

    digitalWrite(
      LED_PIN,
      LOW
    );

    digitalWrite(
      BUZZER_PIN,
      LOW
    );

    Serial.println(
      "ACKNOWLEDGED"
    );

    client.publish(
      ACK_TOPIC,
      "ACKNOWLEDGED"
    );

    delay(500);
  }
}