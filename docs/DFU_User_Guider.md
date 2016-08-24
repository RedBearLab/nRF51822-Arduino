#DFU USER GUIDER
====

##Bootloader version:

###SoftdeviceS130-1.0-Bootloader-Blinky-20160824.hex:

 *This bootloader is made up of softdevice, DFU-bootloader and application.Support all official functions(Wait for more tests).

 *Softdevice version is "s130-nrf51-1.0.0-softdevice.hex".

 *DFU bootloader is based on DFU example in nordic SDK10.0.

 *Application is the example LED blinky(toggle every 1 second).


####How to enter DFU mode:

In DFU mode, LED should blink in 100ms.If no any operations, will exist after 30s.

#####1. Booloader Button

 *Bootloader Button is P0_07.Connecting to GND, then power on or reset.

#####2. DFU service

 *Add dfu service to application(wait for testing).

##How to create .zip 

###Tools

 *nrfutil.exe, get this by installing the "Master Control Panel".

###command

````
nrfutil.exe dfu genpkg --application [app_file_name].hex --application-version [app_version] --softdevice [sd_file_name].hex --bootloader [bootloader_name].hex --dev-type [dev-type] --dev-revision [dev-revision] --sd-req [supported-softdevice-ids] [name of distribution packet].zip
````
 *Note: app_version use default 0xFFFFFFFF,dev-type use default 0xFFFF, dev-revision use default 0xFFFF, supported-softdevice-ids use default 0xFFFE.

 *eg:create a application zip
```
nrfutil.exe dfu genpkg --application D:\Blink.hex --applicati
on-version 0xFFFFFFFF --dev-type 0xFFFF --dev-revision 0xFFFF --sd-req 0xFFFE D:
\Blink.zip
```

 *eg:create softdevice+bootloader zip
```
nrfutil.exe dfu genpkg --application-version 0xFFFFFFFF --softdevice D:\softdevice.hex --bootloader D:\bootloader.hex --dev-type 0xFFFF --dev-revision 0xFFFF --sd-req 0xFFFE D:
\Softdevice_bootloader.zip
```