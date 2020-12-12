# Weather_Station_V3_OneNet_WiFi_LoRa
STM32+ESP8266+SX1278+Onenet的户外气象站制作
================
中文版制作教程请下拉，持续更新中，目前资料还不完整，请持续关注b站GIE工作室获得最新资料。  
The repository is currently under continuous updating, and the existing design is not complete yet.   
This repository contains all design files for an outdoor weather station. It can detect temperature, humidity, atmospheric pressure, TVOC, CO2, PM2.5, UV intensity and its own voltage. Then it can upload these parameters to the OneNet IoT platform through Wifi and http protocols. At the same time, it also provides the source code of an Android APP which can connect the OneNet platform and show these parameters.  
The design is divided into three parts:  
* Outdoor Data Collector
* Indoor Data Display&Uploader
* Android APP
# Outdoor Data Collector
This is the most important part of this design. Sensors and controllers are as follows:  
* STM32L051C8T6 as main controller.  
* SX1278 sends data to the indoor uploader.  
* Two LS Ultra Capacitor 2.8V 3000F in serial to storage the power.  
* Two 9V 3Watts Solar Panels in parallel to power up the whole system.  
* BME680 to get temperature, humidity and atmospheric pressure.  
* SGP30 to get CO2 and TVOC.  
* VEML6070 to get UV intensity.  
* ZPH02 to get PM2.5.  
* ESP8266 to upload the data directly (Optional)  
**Schematic of Data Collector**  
![Schematic of Data Collector](fig/sch_outdoor.jpg)  
**STEP 1 Collect ALL Materials!**  
Use Altium Designer to open the PCB project and check if the circuit is suitable for you. If you don't want any changes, you can directly submit the .PcbDoc files to the factory. And then compare the schematic diagram and the BOM(bill of materials) exported by AltiumDesigner to purchase components. Except the components in the BOM, you also need these:  
LS Ultra Capacitor 2.8V 3000F x2  
Outdoor waterproof box made by ABS or other reliable materials 160x210x100mm x1  
Test Tube 30x100mm(For installing UV sensor) x1  
Desiccant(For filling the test tube)  
Resin waterproof agent  
Venetian box for weather station  
M3 screws and nuts  
Wires and other small things  
**STEP 2 Complete the Welding of the Circuit**  
Update later...  
**STEP 3 Assembly**  
Update later...  
**STEP 4 Program and Debug**  
Notice: Because STM32 works under 1.8V voltage in this design, you need to download the program use the full version of JLINK.  
The program is created on the MDK521 use CUBE MX.
# Indoor Data Display&Uploader
This part of this design can recieve data packages from the outdoor collector. And then upload these data to OneNet using http protocol. In addition, the data can be stored in  128Mbit memory and displayed on a 256x64 OLED screen in real time. After connecting to the computer through USB, the data can be read out through serial port. Controller and other components are as follows:  
* STM32F103CBT6 as main controller.  
* SX1278 receive data from the outdoor collector.  
* Two 18650 lithium batteries to storage the power.    
* ESP8266 to connect the OneNet platform.  
* 256x64 OLED screen to display the data.  
* CH340C to connect the computer.  
* 25Q128 flash to store the data.  
* 3D printing shell.  
**Schematic of Data Uploader**  
![Schematic of Data Uploader](fig/sch_indoor.jpg)  
**STEP 1 Collect ALL Materials!**  
The steps to export BOM are the same as before. In addition to the materials in the BOM, the following items need to be prepared:  
18650 lithium battery x2
256x64 OLED screen x1
3D printing shell(design file also in this repository
M3 screws and nuts  
**STEP 2 Complete the Welding of the Circuit**  
Update later...  
**STEP 3 Assembly**  
Update later...  
**STEP 4 Program and Debug**  
The program is also created on the MDK521 use CUBE MX.
# Android APP
This APP can connect the OneNet and get the data. It is built on Android Studio. Please understand that it only has Chinese version currently.
![APP](fig/fig_app.jpg)  
