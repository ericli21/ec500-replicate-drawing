# EC500: Helping Hand - Replicating a Drawing Using the Robotic Arm

## Contents
***Detect_shape.py*** is a python script that the Raspberry Pi runs with the camera module. It uses openCV as well as paho mqtt libraries. This is script is used for receiving armReady messages, taking foreground and background images, and sending shape information messages. Keep in mind that we used Mosquitto on the Raspberry Pi to set up the broker.

***ESP.ino*** is a C++/Arduino script that the ESP8266 runs for wireless connection. The Raspberry Pi and ESP8266 are connected using a hotspot (see example SSID and password in code), and the ESP uses this to subscribe and publish MQTT messages to the broker on the Raspberry Pi. The ESP is also the master component in the I2C connection with the Uno, and periodically requests the status of the arm from the Uno.

***OWIarm.ino***is a C++/Arduino script that the Uno uses to control the robotic arm and read potentiometer values. It uses the potentiometer readings to move the arm in certain positions for drawing. It also acts as the slave component in the I2C connection with the ESP8266, and handles write events by drawing the shape indicated in the write message. It also handles request events by sending a byte ("A") if the arm is ready.
