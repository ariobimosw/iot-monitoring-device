# iot-monitoring-device
this repository is created from my own final assignment for my college

This IoT project has orientations to monitor certain environment conditions (especially forest fire) by building three arduino tools using DHT11 (temperature & humidity) sensor, fire sensor,
SIM800L GPRS Module, and ThingSpeak Cloud and to give precaution warning through an android software called "GIS" that i have created either.

All those real-time environment condition's datas from the arduino will be sent to the ThingSpeak and afterwards will be fetched by the GIS.

The GIS can show real-time environment conditions from those three arduino tools, determine certain conditions by showing one of red/green/yellow colors 
based on the real-time conditions from each arduino. However, due to the ThingSpeak Cloud that i applied is the "Free" version, the connection
between the Arduino tools and the Cloud has 15 seconds delay.