
nRF51822-Arduino
================

Add support for Arduino IDE to allow compiling nRF51822 firmware.


Requirement
===========

1. nRF51822 deveopment board, here we will use RBL nRF51822 as an example.
2. Arduino IDE version 1.5.8 (currently tested version).
3. Mac OSX 10.9.x (Mavericks) (currently tested version).    <- Read Note 2
   Windows 7 and 8.x

Note 1:
If you have changed the USB interface firmware (MK20 chip) for some reasons, follow the instructions inside the **MK20** folder to restore it in order to use this add-on for Arduino IDE.

Note 2:
For Mac OSX 10.10 (Yosemite), Apple changed the security checking so it will think our MK20 USB dongle firmware is not securer, and it will mount it as read only, thus, you cannot drag the bootloader firmware to it. This will be fixed as soon as possible.

You can still use this add-on if you can use another PC to load the bootloader firmware.


Install nRF51822 Arduino Add-on
===============================

1. Get Arduino IDE version 1.5.8 from Arduino website and install it to your PC

    http://arduino.cc/en/Main/Software

2. Get or clone this repository and put the files to appropriate folder.

  - in your Arduino IDE folder, inside hardware/arduino, you should see
    - avr
    - RBL_nRF51822
    - sam

  - replace the file in hardware/tools/avr/etc/avrdude.conf


Install USB CDC Driver
======================

Get this driver and install it if you are using Windows, so that you can use the USB CDC (Virtual COM Port).

https://mbed.org/media/downloads/drivers/mbedWinSerial_16466.exe

Note that you do not need any driver for OSX and Linux platforms.


Flash with RBL Bootloader
=========================

In the "bootloader" folder, there is a firmware for the RBL nRF51822 board, it allows you to load firmware using Arduino IDE.

To load the bootloader, connect the board to your PC using a micro USB cable, it will prompt a drive, drag the bootloader firmware to the drive.

Note that this can be done only on Windows 7/8 and OSX 10.9 (Mavericks), it will have a fix as soon as possible on OSX 10.10 (Yosemite) via firmware update.


How It Works
============

The RBL nRF51822 board supports loading firmware using USB MSD (Mass Storage Device) method by default. However, to have better user experience as traditional coding, compiling and uploading firmware style using the Arduino IDE, we created a bootloader that makes use USB CDC method to upload firmware.

The board is similar to Ardino Leonardo upload style, when you press "upload" in the Arduino, it will open the USB virtual serial port using 1200bps, the USB chip on the RBL nRF51822 board will reset the nRF51822 chip and it will enter the bootloader, the Arduino IDE will repoen the serial port at 115200bps and the bootloader will wait commands and data from your the Arduino IDE, and upgrade its application area in the flash.


How to Play
===========

1. Select board and serial port

  Select the RBL nRF51822 board in the menu of Arduino IDE and serial port.

    Menu > Tools > Board > RBL nRF51822
  
2. Blink

  To test the board, we suggest you to load the Blink example to see if it works.

    Menu > Examples > 01.Basics > Blink

3. BLE Examples

  There are some BLE examples you can try.
  
    Menu > Examples > BLE_API

  For BLE_HRM, you can use Nordic nRFToolBox App to play. For BLEController, SimpleChat and SimpleControls, you can use RBL BLE Controller App to play.


Limitations
===========

1. Serial Port

    As Arduino does not have flow control in serial port implementation, the Serial port (Pin 0 and 1) is limited to 9600bps since the BLE stack require flow control in order to support higher speed. If you are not going to use BLE, you can use higher speed. 
 
2. Central Role

    The add-on uses SoftDevice S110, it does not support BLE Central role.
