# WardWhisper

An IoT-based smart hospital ward monitoring and silent emergency assistance system designed for hospitals, ICUs, and elderly care centers.

The system enables real-time patient monitoring, silent emergency alerts, visitor tracking, and nurse acknowledgement using ESP32, MQTT, Node-RED, and a touchscreen dashboard.

---

## Features

### Patient Monitoring
- Temperature monitoring using DHT11
- Humidity monitoring using DHT11
- Patient movement detection using PIR sensor
- Emergency assistance request using touch sensor

### Visitor Management
- Entry and exit detection using dual ultrasonic sensors
- Automatic visitor counting
- Visitor status monitoring (Entered / Exited)
- Visitor log integration through Google Forms and Google Sheets

### Silent Emergency Alert System
- Touch sensor acts as emergency assistance button
- Silent nurse alert mechanism
- Emergency acknowledgement using physical push button
- Real-time dashboard updates

### Nurse Dashboard
- Patient profile page
- Medical information page
- Visitor log table
- Ward monitoring page
- Environmental status monitoring
- Emergency alert panel
- Nurse acknowledgement status panel

---

## Hardware Components

### Patient Node (ESP32)

| Component | Purpose |
|------------|------------|
| ESP32 | Main controller |
| DHT11 | Temperature & Humidity monitoring |
| PIR Sensor | Movement detection |
| Touch Sensor | Emergency assistance request |

### Nurse Node (ESP32)

| Component | Purpose |
|------------|------------|
| ESP32 | Alert controller |
| LED | Visual alert |
| Buzzer | Silent nurse notification |
| Push Button | Alert acknowledgement |

### Visitor Counter Node

| Component | Purpose |
|------------|------------|
| ESP32 | Counter controller |
| Ultrasonic Sensor 1 | Entry/Exit sequence detection |
| Ultrasonic Sensor 2 | Entry/Exit sequence detection |

### Display System

| Component | Purpose |
|------------|------------|
| 7-inch HDMI Touch Display | Nurse monitoring dashboard |
| PC / Raspberry Pi | Hosts Node-RED Dashboard |

---

## Software Stack

- ESP32
- Arduino IDE
- MQTT
- EMQX Broker
- Node-RED Dashboard 2.0
- MQTT Explorer
- Google Forms
- Google Sheets
- HTML / CSS / JavaScript

---

## MQTT Topics

### Patient Monitoring

| Topic | Payload |
|---------|---------|
| hospital/ward1/temp | Temperature |
| hospital/ward1/humidity | Humidity |
| hospital/ward1/motion | PATIENT MOVING |
| hospital/ward1/emergency | EMERGENCY |

### Visitor Monitoring

| Topic | Payload |
|---------|---------|
| hospital/ward1/visitor | PERSON_ENTERED / PERSON_EXITED |
| hospital/ward1/visitor_count | Current Count |

### Nurse Acknowledgement

| Topic | Payload |
|---------|---------|
| hospital/ward1/acknowledged | ACKNOWLEDGED |

---

## System Architecture

```text
Patient ESP32
│
├── DHT11
├── PIR Sensor
└── Touch Sensor
        │
        ▼
      MQTT Broker
        ▲
        │
Visitor Counter ESP32
│
├── Ultrasonic 1
└── Ultrasonic 2
        │
        ▼
      Node-RED
        │
        ▼
7-inch Touch Dashboard
        ▲
        │
 Nurse ESP32
 │
 ├── LED
 ├── Buzzer
 └── Push Button
```

---

## Dashboard Pages

### Patient Profile
- Patient information
- Medical information
- Visitor log table

### Ward Monitor
- Temperature
- Humidity
- Visitor count
- Visitor status
- Patient movement
- Emergency alert
- Nurse acknowledgement status

---

## Installation

### Clone Repository

```bash
git clone https://github.com/SIC-ADI/WardWhisper.git
```

### Upload ESP32 Codes

1. Open Arduino IDE
2. Install ESP32 board package
3. Upload:
   - pub_mqtt.ino
   - sub_mqtt.ino
   - ultrasonic.ino

### Import Node-RED Flow

1. Open Node-RED
2. Menu → Import
3. Select:

```text
Node-RED/dashboard_flow.json
```

4. Deploy Flow

### Open Dashboard

```text
http://localhost:1880/dashboard
```

---

## Applications

- Hospitals
- Intensive Care Units (ICU)
- Elderly Care Centers
- Assisted Living Facilities
- Smart Healthcare Monitoring

---

## Future Enhancements

- Camera-based patient monitoring
- AI-based fall detection
- Nurse mobile application
- RFID-based visitor authentication
- Cloud database integration
- Multi-ward support

