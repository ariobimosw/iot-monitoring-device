# iot-monitoring-device
This repository is created from my own final assignment for my college

This IoT project has orientations to monitor certain environment conditions (especially forest fire) by building three arduino tools using DHT11 (temperature & humidity) sensor, fire sensor, SIM800L GPRS Module, and ThingSpeak Cloud and to give precaution warning through an android software called "GIS" that i have created either.

All those real-time environment condition's datas from the arduinos will be sent to the ThingSpeak and afterwards will be fetched by the GIS.

The GIS can show real-time environment conditions from those three arduino tools, determine certain conditions by showing one of red/green/yellow colors 
based on the real-time conditions from each arduino, and navigate us to one of the arduinos via google maps. However, due to the ThingSpeak Cloud that i have applied is the "Free" version, the connection between the Arduino tools and the Cloud has 15 seconds delay.

"GIS_2.aia" file is the codeblock of the android application.
This file must be opened from https://appinventor.mit.edu/ (import) and cannot be opened with our regular software on our device.
