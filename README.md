
nRF51822-Arduino
================

This branch is to provide BLE concurrent Central and Peripheral using Nordic SoftDevice S130. This is a work in progress which adds support for Arduino IDE to allow compiling nRF51822 firmware for fast prototyping and learning how BLE works.

This board support package add-on also provides BLE API for the nRF51822 SoC to act as BLE Central/Peripheral role designed for SoftDevice S130.

* Note: Unless you know how to do with this repository and want to contribute your code, otherwise, please use Arduino IDE's Boards Manager to install the board support package (e.g. for the RBL_nRF51822 and the BLE Nano boards).

Requirement
===========

1. nRF51822 deveopment board, here we will use RBL nRF51822 as an example.

* Note: if you are using the BLE Nano, you also need the MK20 USB dongle.

2. PC with one of the following OS:
    - Mac OSX 10.11.2 (current tested version), OSX 10.9.x, 10.10.x should also work.
    - Windows 10 (current tested version), Windows 7 and 8.x should also work.
    - Linux (current tested on Ubuntu 14.04).

3. Arduino IDE version 1.6.6 (current tested version).

4. Board support package add-on 1.0.3 (current test version).

* Note: If you have changed the USB interface firmware (MK20 chip) for some reasons, follow the instructions inside the **MK20** folder to restore it in order to use this add-on for Arduino IDE.


Install Board Support Package Add-on
=====================================================

1. Get Arduino IDE version 1.6.6 from Arduino website and install it to your PC
	
	http://arduino.cc/en/Main/Software

2. Start the IDE and from the Menu, click "Preference...", add the following line to "Additional Boards Manager URLs"
 
	https://redbearlab.github.io/arduino/package_redbearlab_index.json  
  
3. Install the "RedBearLab nRF51822 Boards" add-on via Boards Manager from the Menu: Tools -> Board -> Boards Manager ... 

* Note: current board support package version is 1.0.3.


Install USB CDC (Serial) Driver (for Windows only)
==================================================

Get this driver and install it if you are using an Windows PC, so that you can use the Serial port for uploading sketches in Arduino IDE.

[USB CDC Driver](https://mbed.org/media/downloads/drivers/mbedWinSerial_16466.exe)

* Note that you do not need any driver for OSX and Linux platforms.


Flash with RBL Bootloader
=========================

In the "bootloader" folder, there is a firmware (bootloader.hex) for the RBL nRF51822 board, it allows you to load firmware using Arduino IDE.

To load the bootloader, connect the board to your PC using a micro USB cable, it will prompt a drive, drag the bootloader firmware to the drive.

Note that the bootloader has OTA feature and you can follow the OTA session for instructions.


How It Works
============

The RBL nRF51822 board supports loading firmware using USB MSD (Mass Storage Device) method by default. However, to have better user experience as traditional coding, compiling and uploading firmware style using the Arduino IDE, we created a bootloader that makes use USB CDC method to upload firmware.

The board is similar to Ardino Leonardo upload style, when you press "upload" in the Arduino, it will open the USB virtual serial port using 1200bps, the USB chip on the RBL nRF51822 board will reset the nRF51822 chip and it will enter the bootloader, the Arduino IDE will repoen the serial port at 115200bps and the bootloader will wait commands and data from your the Arduino IDE, and upgrade its application area in the flash.


How to Play
===========

1. Select board and serial port

  Select the RBL nRF51822 board in the menu of Arduino IDE and serial port.

    Menu > Tools > Board > RBL nRF51822
    
  If you are using our BLE Nano board,

    Menu > Tools > Board > BLE Nano
  
2. Blink

  To test the board, we suggest you to load the Blink example to see if it works.

    Menu > Examples > 01.Basics > Blink

3. BLE Examples

  There are some BLE examples you can try.
  
    Menu > Examples > BLE_API

  #### BLE_HRM
   
  This is an Heart Rate Monitor demo, it simulates Heart Rate detection. You can use Nordic nRFToolBox App to play. For BLEController, SimpleChat and SimpleControls, you can use RBL BLE Controller App to play.

  #### BLE_Beacon
    
  This is to implement Apple's iBeacon, you can try it with our BeaconTool available in the AppStore for iOS and PlayStore for Android.

  #### SimpleControls
  
  This sample shows how to define your own protocol and to control the I/O of the board from a Central (e.g. iPhone) using BLE Controller Apps.
  
  #### SimpleChat
  
  This sample allows you to send text message from the board to your central device such as iPhone or Android using our BLE Controller Apps.
  
  #### BLEController

  This example allows you to use our BLE Controller Apps (available for iOS and Android) to control the pin state such as High, Low, PWM, Analog, etc.
  
  #### nRF51822_UART
  
  This example allows you to exchange data with your central device (e.g. iPhone 5) and the data will be redirected to the UART.


OTA
===

The bootloader allows you to load firmware Over-the-air. Everytime, if you reset the board by hitting the button on the board, the bootloader will broacast the OTA service via BLE for 5 sec waiting for a connection to be made. Otherwise, it will start the pre-loaded firmware.

You can use the "export" feature from Arduino IDE 1.6.5 to get the firmware for OTA.

Menu -> Sketch -> Export Compiled Binary

Try the blink example, the firmware is named "Blink.cpp_OTA.BLE_Nano.hex".

You can send this firmware to your phone, e.g. iPhone, using Air-Drop and use Nordic "nRF Toolbox" App to open it, then select file type "application" and select a device (you should see DFU_S130_V1.0 in the device list). Select it and click upload, the App will transfer the firmware to the board via BLE.


Limitations
===========

1. Serial Interface

    As Arduino does not have flow control in serial port implementation, the Serial port (Pin 0 and 1) is limited to 9600bps since the BLE stack require flow control in order to support higher speed. If you are not going to use BLE, you can use higher speed. 

2. Compatibility

    It is not easy for the nRF51822 chip (ARM 32-bit) to be 100% compatible (the SoC has to do BLE tasks and ensure low power) with the Atmel chips (AVR 8-bit), the underlying driver from Nordic for the peripherals may also have their limitations.


License
=======

Copyright (c) 2013-2015 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

