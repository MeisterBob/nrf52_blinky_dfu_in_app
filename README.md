# nRF52 non blocking blinky with DFU_IN_APP

This piece of code is designed to run on an nRF52840-DK (PCA10056). DFU is transfered via UART, you can view the debug output in JLinkRTTViewer.

To use it you need to set the path to your nRF5-SDK-17 in line 5 and the location of your `dfu_public_key.c` in line 14 of the `Makefile`. Compile it using `make` and flash with `make flash_mbr flash`. Ofcourse you need to flash a bootloader to your DK (if you want to use the BLE Bootloader you need to adapt the FLASH memory location in `dfu_in_app_blinky_gcc_nrf52.ld`). You can run the firmware update using nrfutil or `make serialupdate`, that will generate the dfu zip file and send it to `/dev/ttyACM0`. The serial port can be changed in line 254 of the `Makefile`