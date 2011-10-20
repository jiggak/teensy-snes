Convert your SNES gamepad into a generic USB joystick.
Read [more](http://www.slashdev.ca/2011/10/20/snes-usb-gamepad/).

Building
--------

Install toolchain:

	pacman -S avr-gcc

... and [teensy loader](http://www.pjrc.com/teensy/loader_cli.html)

	yaourt teensy-loader-cli

... then build by running *make*. Load firmware to teensy:

	teensy-loader-cli -w -mmcu=atmega32u4 gamepad.hex

Reference
---------
- [Inspiration](http://www.instructables.com/id/SNES-Teensy-USBPS3-Gamepad/)
- [Teensy](http://www.pjrc.com/teensy/)