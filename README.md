# Captive Portal Project

This project implements a captive portal for ESP8266 and ESP32 devices. The captive portal allows users to connect their IoT devices to a WiFi network through a web interface.

## Features

- **Access Point Mode**: The device starts in AP mode if no saved WiFi credentials are found or if it fails to connect to the saved network.
- **Captive Portal**: A web interface is provided for users to enter their WiFi credentials.
- **WiFi Connection**: The device attempts to connect to the provided WiFi credentials and provides feedback on the connection status.
- **Fallback to AP Mode**: If the connection to the WiFi network is lost, the device reverts to AP mode.

## File Structure

- **ESP8266**: Contains the captive portal implementation for ESP8266 devices.
  - `captivePortal.ino`: Main code for the ESP8266 captive portal.
- **ESP32**: Contains the captive portal implementation for ESP32 devices.
  - `captivePortal.ino`: Main code for the ESP32 captive portal.
- **exampleView.html**: HTML file for the captive portal web interface.

## Setup Instructions

1. **Hardware Requirements**:
   - ESP8266 or ESP32 device
   - USB cable for programming the device

2. **Software Requirements**:
   - Arduino IDE
   - ESP8266 or ESP32 board package installed in Arduino IDE

3. **Installation**:
   - Clone this repository to your local machine.
   - Open the appropriate `.ino` file (ESP8266 or ESP32) in the Arduino IDE.
   - Connect your device to your computer via USB.
   - Select the correct board and port in the Arduino IDE.
   - Upload the code to your device.

4. **Usage**:
   - After uploading the code, the device will start in AP mode if no saved WiFi credentials are found.
   - Connect to the device's AP (SSID: `@codewithwan_device`).
   - Open a web browser and navigate to `http://192.168.4.1`.
   - Enter your WiFi credentials and click "Connect".
   - The device will attempt to connect to the provided WiFi network and provide feedback on the connection status.
