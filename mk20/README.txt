
In case you have changed the USB interface firmware inside the MK20 chip and want to restore to default one or upgrade to lastest version, follow the steps below:

1. Press and hold the reset button on your nRF51822 board 

2. Connect it to an USB port of your PC 

3. Release the button and the LED will keep flashing (slowly) 

4.1 For Windows, just drag the firmware (.bin) to the MSD drive (e.g. E:)

4.2 For Linux or Mac OSX (10.9.x), use the follow command from the Terminal:

    Usage:
	dd if=src_firmware.bin of=/Volumes/BOOTLOADER/dst_firmware.bin conv=notrunc

    Example:
	dd if=BLENano_MK20.bin of=/Volumes/BOOTLOADER/BLENano_MK20.bin conv=notrunc

4.3 For Mac OSX (10.10.x, Yosemite):

    sudo mount -u -w -o sync /Volumes/BOOTLOADER; cp -X ~/Desktop/BLENano_MK20.bin /Volumes/BOOTLOADER/

Note: Use RBL_nRF51822_MK20.bin for the RBL nRF51822 board and use BLENano_MK20.bin for the BLE Nano board

5. After that, the LED should keep flashing (very fast this time) 

6. Remove the board from the USB port 

7. Reconnect it to the USB port
