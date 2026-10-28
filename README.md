# ESP32-S3 Web Server - TinyML Engine Monitoring

This is a lightweight web server using ESP_IDF for the ESP32-S3 microcontroller. This project is part of a TinyML-based system for acoustic fault detection in internal combustion engines. The ESP32-S3 hosts a local web dashboard intended to display the machine learning predictions.

The system operates in Wi-Fi Access Point (AP) mode, allowing users to access the interface directly from a smartphone or computer, without an external router or internet connection.

The `index.html` file is embedded into the firmware during compilation using the `EMBED_FILES` option in Cmake.

## Build and Flash

Connect the ESP32-S3 to your computer and run:

```bash
idf.py set-target esp32s3
idf.py build
idf.py flash monitor 
```

Once initialized, the serial monitor should display the Access Point IP address and HTTP server status.

## Access the Web Interface

1. On your smartphone or computer, open the Wi-Fi settings.

2. Connect to the MotorMonitor network using the configured password.

3. Open a web browser and navigate to: http://192.168.4.1


An internet connection is not required. The device creates its own local wireless network.