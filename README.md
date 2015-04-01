
nRF51822-Arduino
================

Add support for Arduino IDE to allow compiling nRF51822 firmware.

This add-on also provides BLE API for nRF51822 SoC to act as BLE Peripheral role designed for SoftDevice S110 version 6.0.


Requirement
===========

1. nRF51822 deveopment board, here we will use RBL nRF51822 as an example.
2. Arduino IDE version 1.6.2 (currently tested version).
3. PC with one of the following OS:
    - Mac OSX 10.9 (Mavericks) / 10.10 (Yosemite) (currently tested version).    <- Read Note 2
    - Windows 7 and 8.x (currently tested version).
    - Linux (currently tested on Ubuntu 14.04).

Note 1:
If you have changed the USB interface firmware (MK20 chip) for some reasons, follow the instructions inside the **MK20** folder to restore it in order to use this add-on for Arduino IDE.

Note 2:
For Mac OSX 10.10 (Yosemite), Apple changed the security checking so it will think our MK20 USB dongle firmware is not securer, and it will mount it as read only, thus, you cannot drag the bootloader firmware to it. This will be fixed as soon as possible.

A workaround using Terminal:
sudo mount -u -w -o sync /Volumes/MBED ; cp -X bootloader.hex /Volumes/MBED/

Thanks to @okano for this and you can use his droplet to do drag and drop without using the Terminal:
https://developer.mbed.org/users/okano/notebook/mbed-on-yosemite/?c=14179


Install nRF51822 Arduino Add-on
===============================

1. Get Arduino IDE version 1.6.2 from Arduino website and install it to your PC

    http://arduino.cc/en/Main/Software
    
2. Install the "Arduino SAM Boards" add-on via Board Manager: Tools -> Board -> Board Manager ... 

3. Get or clone this repository and copy the "RBL" folder from "arduino-1.5.x/hardware" into  "~/Documents/Arduino/hardware" (if the "hardware" folder does not exist, create it)

Mac OSX:
Documents > Arduino

Windows:
C: > Users > YourName > Documents > Arduino


Install USB CDC Driver
======================

Get this driver and install it if you are using Windows, so that you can use the USB CDC (Virtual COM Port).

https://mbed.org/media/downloads/drivers/mbedWinSerial_16466.exe

Note that you do not need any driver for OSX and Linux platforms.


Flash with RBL Bootloader
=========================

In the "bootloader" folder, there is a firmware for the RBL nRF51822 board, it allows you to load firmware using Arduino IDE.

To load the bootloader, connect the board to your PC using a micro USB cable, it will prompt a drive, drag the bootloader firmware to the drive.

Note that for OSX 10.10 (Yosemite), you need to do this by using Terminal (will be fixed as soon as possible):
sudo mount -u -w -o sync /Volumes/MBED ; cp -X bootloader.hex /Volumes/MBED/

Or refer to this:
https://developer.mbed.org/users/okano/notebook/mbed-on-yosemite/?c=14179


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


Limitations
===========

1. Serial Port

    As Arduino does not have flow control in serial port implementation, the Serial port (Pin 0 and 1) is limited to 9600bps since the BLE stack require flow control in order to support higher speed. If you are not going to use BLE, you can use higher speed. 
 
2. Central Role

    The add-on uses SoftDevice S110, it does not support BLE Central role.


ToDo
====

1. Support SoftDevice S110 version 7.1
2. Bootloader for both OTA and USB CDC


License
=======

Copyright (c) 2013-2015 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

