
# nRF51822-Arduino
===

This branch is to provide BLE concurrent Central and Peripheral using Nordic SoftDevice S130 and also provides support for the Arduino IDE to compile nRF51822 firmware. This aims to provide fast prototyping and learning the IoT development using the Bluetooth Low Engergy (BLE) technology.

This board package add-on also provides BLE API for the nRF51822 SoC to act as BLE Central/Peripheral role designed for SoftDevice S130.

* Note: Unless you know how to do with this repository and want to contribute your code, otherwise, please use Arduino IDE's Boards Manager to install the board support package (e.g. for the RBL_nRF51822 and the BLE Nano boards).


# Features
===

* Adds support for the Arduino IDE enables compiling firmwares for the RBL_nRF51822 and BLE Nano development boards.
* Upload the OTA bootloader using OpenOCD tool via the CMSIS-DAP debug interface.
* Upload sketches using OpenOCD.
* Provides BLE API for SoftDevice S130 with concurrent BLE Central and Peripheral role support.


# Requirements
===

1. nRF51822 deveopment board, here we will use the RBL nRF51822 as an example.

	* Note: if you are using the BLE Nano, you also need the MK20 USB dongle, otherwise, you can only load sketches using the OTA method. 

2. PC with one of the following OS:
    - Mac OSX 10.11.4 (current tested version), OSX 10.9.x, 10.10.x should also work.
    - Windows 10 (current tested version), Windows 7 and 8.x should also work.
    - Linux (current tested on Ubuntu 14.04).

3. Arduino IDE version 1.6.9 (current tested version).

4. Board support package add-on 1.0.6 (current tested version).

	* Note 1: If you have changed the USB interface firmware (MK20 chip) for some reasons, follow the instructions inside the **[interface/mk20](interface/mk20)** folder to restore it in order to use this add-on for Arduino IDE.


# Getting Started
===

## Install Board Package Add-on

1. Get Arduino IDE from Arduino website and install it to your PC
	
	http://arduino.cc/en/Main/Software

2. Start the IDE and from the Menu, click "Preference...", add the following line to "Additional Boards Manager URLs"
 
	https://redbearlab.github.io/arduino/package_redbearlab_index.json  
  
3. Install the "RedBearLab nRF51822 Boards" add-on via Boards Manager from the Menu: Tools -> Board -> Boards Manager ... 


## Install USB CDC (Serial) Driver (Windows only)

Get this driver and install it if you are using an Windows PC, so that you can use the Serial port for uploading sketches in Arduino IDE.

[USB CDC Driver](https://mbed.org/media/downloads/drivers/mbedWinSerial_16466.exe)

* Note that you do not need any driver for OSX and Linux platforms.

## Add udev rules (Linux only)

Copy file "98-daplink.rules" to path "/etc/udev/rules.d/". Then restart or reloaded with 

    $ sudo udevadm control --reload-rules

We thank bapowell & kdsoo for solving this problem.
[openocd error on Linux](https://github.com/RedBearLab/nRF51822-Arduino/issues/73)

## Fix the upload problem on Linux 64bit system

    sudo apt-get install lsb-core

    sudo apt-get install libudev1:i386

    sudo ln -sf /lib/x86_64-linux-gnu/libudev.so.1 /lib/x86_64-linux-gnu/libudev.so.0

## Burn Bootloader

The nRF51822 boards are proloaded the bootloader already. Normally, you can skip this step. You can check with this:

	After reset/power up the board, use the iOS App - LightBlue or the Android App - B-BLE to see if the bootloader works or not, since it will broadcast our OTA service.

In the "bootloader" folder, there is a firmware (bootloader.hex) for the RBL nRF51822 board, it allows you to load Arduino sketch using the Arduino IDE.

To load the bootloader, connect the board to your PC using a micro USB cable, it will prompt a drive, drag the bootloader firmware to the drive.

If you used mbed drag & drop method for firmware uploading before, you will need to reflash the bootloader back in order to use Arduino upload method (via Serial).

Note that the bootloader has OTA feature and you can follow the OTA session for instructions.


## How to Play

1. Select board and serial port

  Select the RBL nRF51822 board from the menu of the Arduino IDE,

    Menu > Tools > Board > RBL nRF51822
    
  If you are using the BLE Nano board,

    Menu > Tools > Board > BLE Nano
  
  Select the serial port for print out with the USB Interface Dongle,
  
    Menu > Tools > Port > [you board serial port name]
  
2. Blink

  To test the board, we suggest you to load the Blink example to see if it works.

    Menu > File > Examples > 01.Basics > Blink

  Use the upload icon to load the sketch to your board.
  
3. BLE Examples

  There are some BLE examples you can try.
  
    Menu > File > BLE_Examples

  #### BLE_HRM
   
  This is an Heart Rate Monitor demo, it simulates Heart Rate detection. You can use Nordic nRFToolBox App to play.

  #### BLE_Beacon
    
  This is to implement Apple's iBeacon, you can try it with our BeaconTool available in the AppStore for iOS and PlayStore for Android.

  #### SimpleControls
  
  This sample shows how to define your own protocol and to control the I/O of the board from a Central (e.g. iPhone) using the BLE Controller App.
  
  #### SimpleChat
  
  This sample allows you to send text message from the board to your central device such as iPhone or Android using the BLE Controller App.
  
  #### BLEController

  This example allows you to use the BLE Controller App (available for iOS and Android) to control the pin state such as High, Low, PWM, Analog, etc.
  
  #### nRF51822_UART
  
  This example allows you to exchange data with your central device (e.g. iPhone 5) and the data will be redirected to the UART.


# OTA
===

The bootloader allows you to load firmware Over-the-air, for example, if you do not have the MK20 USB interface dongle.

Everytime, if you reset the board by hitting the button on the board, the bootloader will broacast the OTA service via BLE for 5 sec waiting for a connection to be made. Otherwise, it will start the pre-loaded firmware.

You can use the "export" feature from Arduino IDE to get the firmware for OTA.

Menu -> Sketch -> Export Compiled Binary

Try the blink example, the firmware is named "Blink.cpp_OTA.BLE_Nano.hex".

You can send this firmware to your phone, e.g. iPhone, using Air-Drop and use Nordic "nRF Toolbox" App to open it, then select file type "application" and select a device (you should see DFU_S130_V1.0 in the device list). Select it and click upload, the App will transfer the firmware to the board via BLE.


# Limitations
===

1. Serial Interface

    As Arduino does not have flow control in serial port implementation, the Serial port (Pin 0 and 1) is limited to 9600bps since the BLE stack requires flow control in order to support higher speed. If you are not going to use BLE, you can use higher speed. 

2. Compatibility

    It is not easy for the nRF51822 chip (ARM 32-bit) to be 100% compatible (the SoC has to do BLE tasks and ensure low power) with the Atmel chips (AVR 8-bit), the underlying driver from Nordic for the peripherals may also have their limitations.


# License
===

Copyright (c) 2013-2016 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
