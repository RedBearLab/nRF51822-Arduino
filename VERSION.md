
## Version

This is about the board support package version.

#### Current Version

#### Version: 1.0.8

* Fix bug of Example "BLEController" : "reporting pin's status too fast for andriod".
* Fix function "shiftOut()" : "Not generate clock correctly".
* Fix the bug "Can't use DFUService".
* Fix "Wire::endTransmission()".

#### Verison: 1.0.7

* Use SandeepMistry's openocd to instead of arduino openocd
* Fixed bug "Can't upload on macosx"
* Change file "nrf_ble.h" to "ble.h"

##### kown issue

* Upload problem on Linux 64bit, fix it as follow:
```
    sudo apt-get install lsb-core

    sudo apt-get install libudev1:i386

    sudo ln -sf /lib/x86_64-linux-gnu/libudev.so.1 /lib/x86_64-linux-gnu/libudev.so.0
```
* Can't use DFUService.
* Function "shiftOut" : "Not generate clock correctly".
* Example "BLEController" not compatible with andriod.

#### Version: 1.0.6

* Update libraries : ble-master, ble-nrf51822-master,mbed
* Update examples, add more comments
* Using openocd tools to instead of avrdude for uploading
* Buring bootloader using DAPLink
* Fixed bug "If don't call Serial.begin(),analogRead() would not work well"
* Fixed bug "launchServiceDiscovery() doesn't discover all services and characteristics"

##### kown issue

* Can't upload on macosx


#### Version: 1.0.5

* Fixed BLE Controller Sketch





