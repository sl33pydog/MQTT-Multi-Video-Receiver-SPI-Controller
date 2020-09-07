# MQTT-Multi-Video-Receiver-SPI-Controller

This is an esp32 IDE program that connects to an MQTT server and changes the frequncies of a video receiver like an RX5808 over SPI.

This program really is intended to be used with RotorHazard.  An open source timing system that uses a raspberry pi, nodes and video receivers with RSSI output.
RotorHazard uses Mosquitto to send messages to other nodes for OSD/Frequency purposes.

Use arduino and ESP32 Dev module to flash onto a ESP32 Wroom.

Connect Pin 23 to Mosi (Ch 1 on an RX5808) all video receivers
Connect Pin 18 to SCLK (Ch 3 on an RX5808) all video receivers
Connect Pin 13 to SS (Ch 2 on an RX5808) first video receiver 
Connect Pin 12 to SS (Ch 2 on an RX5808) second video receiver 
Connect Pin 14 to SS (Ch 2 on an RX5808) third video receiver 
Connect Pin 27 to SS (Ch 2 on an RX5808) forth video receiver 
Connect Pin 26 to SS (Ch 2 on an RX5808) fifth video receiver 
Connect Pin 25 to SS (Ch 2 on an RX5808) sixth video receiver 
Connect Pin 33 to SS (Ch 2 on an RX5808) seventh video receiver 
Connect Pin 32 to SS (Ch 2 on an RX5808) eighth video receiver

