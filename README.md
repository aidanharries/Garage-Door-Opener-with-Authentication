# IoT: Garage Door Opener with Authentication

**Final Project for ECE 631**

**Authors:** Aidan Harries, Andy Freshnock

## Introduction

This project aims to design and implement a sophisticated garage door opener system with authentication capabilities using a combination of hardware and software components. The system enhances security and functionality by allowing door access only to authenticated users and providing real-time updates on the door's status and the proximity of a vehicle to the wall within the garage.

## Project Scope and Goals

- **Objective:** Enhance security by allowing access only to authenticated users.
- **Features:** Real-time updates on door status and vehicle proximity.

## Hardware Used

- **ESP32 Development Boards:** Two boards, one interfaced with an ultrasonic sensor and the other with a hall effect sensor.
- **Raspberry Pi 4:** Served as the control unit for the NFC card reader and output sound notifications.
- **Digital Ocean MQTT Broker:** Facilitated message communication between hardware components.
- **PN532 NFC Card Reader:** Authenticated user access based on NFC card scans.
- **49E Hall Effect Sensor and HC-SR04 Ultrasonic Sensor:** Detected door status and measured vehicle distance to the wall.
- **Miscellaneous Components:** Wires, a speaker with an Aux chord, a laptop for observation/programming, and an oscilloscope for testing.

## Software Used

- **C Language:** Programmed the ESP32 microcontrollers.
- **Python:** Programmed the Raspberry Pi for NFC authentication and sound playback.
- **Arduino Development Environment:** Used for writing, debugging, and uploading C code to the ESP32s.
- **MQTT Library:** Enabled communication with the Digital Ocean MQTT broker.
- **Freeboard Dashboard:** Displayed real-time information from the system.
- **NFC Library (PN532):** Supported the NFC card reading processes.
- **PuTTY and HiveMQ:** Used for secure SSH communication and initial MQTT communication tests.

## System Features

- **NFC Authentication:** Validates user access with the NFC card reader.
- **Distance Measurement:** Uses the ultrasonic sensor to determine the vehicle's distance from the wall.
- **Door Status Monitoring:** Utilizes the hall effect sensor to provide real-time door status.
- **Communication and Notification:** Sends data over MQTT and updates the Freeboard dashboard.

## Design Partitioning

### Hardware Partitions

1. **ESP32 Development Boards:** One for distance measurement, the other for door status monitoring.
2. **Raspberry Pi 4:** Managed NFC authentication and sound notifications.
3. **Sensors and Additional Components:** Included NFC card reader, hall effect sensor, ultrasonic sensor, speaker, wires, and an oscilloscope for testing.

### Software Partitions

1. **Setup:** Initialization of WiFi, MQTT connections, and pin configurations.
2. **Main Loop:** 
   - **Ultrasonic Sensor:** Managed distance calculations.
   - **Hall Effect Sensor:** Monitored door status.
   - **NFC Reader:** Handled access control and sound notifications.
3. **Interrupt Service Routine (ISR):** Handled echo pin changes for the ultrasonic sensor.

### Functional Partitions

- **Authentication via NFC:** Validates user access.
- **Distance Measurement:** Uses ultrasonic sensor for vehicle proximity.
- **Door Status Monitoring:** Uses hall effect sensor for door status.
- **Communication and Notification:** Updates Freeboard dashboard and sends MQTT messages.

## Design Decisions

- **Moving Average Filter:** Smooths out sensor data.
- **PWM (Pulse Width Modulation):** Controls the ultrasonic sensor's pulse emission.
- **Circular Buffer:** Manages the stream of distance data.
- **ISR (Interrupt Service Routine):** Efficiently handles echo responses from the ultrasonic sensor.

## Design Issues / Limitations

- **Bluetooth Connectivity Problems:** Switched to AUX connection for audio output due to unstable Bluetooth.
- **Limited Speaker Output Power:** Audio output from the speaker was quieter than expected, considering using an external amplifier for future improvements.

## Testing

- **PWM Test:** Verified the PWM signal for the ultrasonic sensor.
- **Ping Sensor Distance Test:** Tested distance measurements at fixed positions.
- **NFC Authentication Test:** Validated the NFC system using different NFC cards.
- **Hall Effect Sensor Test:** Simulated different door statuses.
- **Blinking LED Test:** Verified the LED blink rate based on distance measurements.

## Analysis and Future Improvements

- **Enhance Ultrasonic Sensor Accuracy:** Adjust sampling methods or revise the distance calculation algorithm.
- **Improve Audio Output:** Consider integrating an external amplifier for better sound clarity.

## Conclusion

The project provided hands-on experience in designing and troubleshooting an IoT system. Despite challenges, such as Bluetooth instability and distance measurement inaccuracies, the system successfully integrated multiple components to enhance garage security and functionality. Future improvements will focus on sensor accuracy and audio output enhancements.
