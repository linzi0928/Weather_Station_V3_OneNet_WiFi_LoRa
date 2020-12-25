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
The program is created on the MDK521 use CUBE MX. The data collector and the uploader are connected by sx1278 in 433MHz frequency band.In order to save power, when the collector completes a data acquisition of sensors, it will pack all the data and send it in 433MHz frequency band only once. So the uploader needs to listen all the time to avoid missing packets.The communication protocol between collector and uploader is shown in the chart below.  

|  Byte  | 0-3 | 4-7 | 8-11 | 12-15 | 16-16 | 17-20 | 21-22 | 23-24 | 25-26 | 27-30 |
| :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
| Content | Temperature | Humidity | Pressure | Gas resistance | VOC Level | PM2.5 | UV | CO2 | TVOC | Voltage |
| Type | Float | Float | Float | Float | u8 | u32 | u16 | u16 | u16 | Float |
  
# Indoor Data Display&Uploader
This part of this design can recieve data packages from the outdoor collector. And then upload these data to OneNet using http protocol. In addition, the data can be stored in  128Mbit memory and displayed on a 256x64 OLED screen in real time. After connecting to the computer through USB, the data can be exported through serial port. Controller and other components are as follows:  
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
IPEX-SMA cable x2  
Knob handle for ec11 x1  
10pin 1mm 10cm reverse FPC cable  x1  
3D printing shell(design file also in this repository  
M3 screws and nuts  
Dupont lines
**STEP 2 Complete the Welding of the Circuit**  
![PCB of indoor uploader](fig/fig_indoor_pcb.jpg)  
There is no special process for indoor uploader welding. The specific parameter of each component can be obtained by look at the schematic diagram.We recommend welding the chips, resistors and capacitors at first, then welding higher components, and finally welding the plug-ins, which can make the welding process more convenient.After the welding is completed, power on to test whether it is working properly. Normally, there will be abnormal work caused by improper welding.     
**STEP 3 Program and Debug**  
Same as the data collector, the program is also created on the MDK521 use CUBE MX and we also recommend using JLINK to download and debug the program. If you do not want to add or remove any functions, you can directly download the program into the microcontroller. 
Wifi name, password and various information required to access the OneNet platform can be written through the USB serial port using AT command.Use a micro usb cable to connect the computer and the uploader. If you are using this kind of USB serial port for the first time, you may need to install the CH340 driver.Then open a serial port assistant and select the port number that the uploader is connected to. Then you can send AT commands through serial port assistant to write configuration information.All available AT commands are shown below(Note: All commands need to end with a newline (\r\n)):  
* AT+EXP  
This command is used to query and export the weather information stored in the uploader.  
Using AT+EXP?, you can query the storage address of the latest data. The address indicates the location where the weather information is stored in the flash, starting from 0x10000 (0x00000-0x10000 are reserved for storing configuration information). Each time a piece of weather information is stored, the address will increase by 64 bytes, which is Hexadecimal 0x40, for example, when the first message is stored, the latest address will change from 0x10000 to 0x10040, and so on.  
Using AT+EXP=<addr> you can export all weather information starting from this address to the latest. If addr=10000, all weather information can be exported.(Example: AT+EXP=10000)  
* AT+ERS  
This command can be used to erase all weather information. For safety, you need to type and send this command twice to complete the deletion.  
* AT+CWJAP  
This command can be used to configure the ssid and password of the Wifi connected to the uploader.  
Using AT+CWJAP=<WiFi SSID>,<WiFi password> to achieve it. For example:AT+CWJAP=TP-Link-xxx,88888888. Note that the SSID cannot be longer than 21 letters, and the password cannot be longer than 20 letters, otherwise the configuration will fail.  
* AT+CMSET  
This command can be used to set the access information of the OneNet platform.  
Using AT+CMSET=<dev_id>,<API-key> to achieve it. For example:AT+CMSET=657284563,SkFGTtTMpEFf=xG580jwowTZ9r0=.
  
**STEP 4 Assembly**  
Because it is not easy to modify the circuit or program after assembly, we recommend installing it into the shell after debugging.You can paint the shell to your desired color before assembling if you like. In addition, you can also use the DuPont cable to lead the debug port to the side of the circuit board, so that you can easily modify the program even after assembly.  
![indoor uploader](fig/fig_indoor.jpg)  
The first step in assembly is to install the screen. First, insert the FPC cable into the screen, and then fold the FPC into a 90-degree angle so that it can be drawn out from the side. Then put the screen into the shell diagonally upwards, so that the PCB of the screen is stuck in the gap at the top of the shell and allow the screen expose completely. Then use hot melt glue to simply fix the bottom and sides of the screen.  
Then insert the circuit board with the battery into the shell. After installing the back plate, the assembly is complete.
# Android APP
This APP can connect the OneNet and get the data. It is built on Android Studio. Please understand that it only has Chinese version currently.
![APP](fig/fig_app.jpg)  
