/* Teensy SNES Gamepad
 * Copyright (C) 2011 Josh Kropf <josh@slashdev.ca>
 *
 * Based on keyboard example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/usb_keyboard.html
 * Copyright (c) 2008 PJRC.COM, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <avr/io.h>
#include <util/delay.h>
#include "usb_gamepad.h"

#define LED_CONFIG  (DDRD |= (1<<6))
#define LED_OFF     (PORTD &= ~(1<<6))
#define LED_ON      (PORTD |= (1<<6))

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

#define SNES_CLOCK_PIN	(1 << 0)
#define SNES_LATCH_PIN	(1 << 1)
#define SNES_DATA_PIN	(1 << 2)

#define SNES_DDR	DDRD
#define SNES_PORT	PORTD
#define SNES_PINS	PIND

void read_snes_state() {
	SNES_PORT &= ~SNES_LATCH_PIN; // set latch low

	// 12 cycles to read 4 directions and 8 buttons
	for (int i=0; i<12; i++) {
		// pulse clock pin low/high
		SNES_PORT &= ~SNES_CLOCK_PIN;
		SNES_PORT |= SNES_CLOCK_PIN;

		uint8_t on = !(SNES_PINS & SNES_DATA_PIN);

		switch (i) {
		case 0: gamepad_state.b_btn = on; break;
		case 1: gamepad_state.y_btn = on; break;
		case 2: gamepad_state.select_btn = on; break;
		case 3: gamepad_state.start_btn = on; break;

		case 4: if (on) gamepad_state.y_axis = 0x00; break;
		case 5: if (on) gamepad_state.y_axis = 0xff; break;
		case 6: if (on) gamepad_state.x_axis = 0x00; break;
		case 7: if (on) gamepad_state.x_axis = 0xff; break;

		case 8: gamepad_state.a_btn = on; break;
		case 9: gamepad_state.x_btn = on; break;
		case 10: gamepad_state.l_btn = on; break;
		case 11: gamepad_state.r_btn = on; break;
		}
	}

	SNES_PORT |= SNES_LATCH_PIN; // set latch high
}

int main(void) {
	// set for 16 MHz clock
	CPU_PRESCALE(0);

	// good explenation of how AVR pins work:
	// http://www.pjrc.com/teensy/pins.html

	SNES_DDR  |= SNES_CLOCK_PIN; // configure clock + latch pins for output
	SNES_DDR  |= SNES_LATCH_PIN;
	SNES_PORT |= SNES_CLOCK_PIN; // high output
	SNES_PORT |= SNES_LATCH_PIN;

	SNES_DDR  &= ~SNES_DATA_PIN; // configure data pin for input
	SNES_PORT &= ~SNES_DATA_PIN; // normal input (ie: no pullup)

	LED_CONFIG;
	LED_ON; // power up led on startup for 1 sec

	// Initialize the USB, and then wait for the host to set configuration.
	// If the Teensy is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured()) /* wait */ ;

	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	_delay_ms(1000);

	LED_OFF;

	while (1) {
		usb_gamepad_reset_state();
		read_snes_state();
		usb_gamepad_send();
	}
}
