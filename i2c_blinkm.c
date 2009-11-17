/*
	Copyright (c) 2009, Scott Ellis
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
		* Redistributions in binary form must reproduce the above copyright
		  notice, this list of conditions and the following disclaimer in the
		  documentation and/or other materials provided with the distribution.
		* Neither the name of the <organization> nor the
		  names of its contributors may be used to endorse or promote products
		  derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY <copyright holder> ''AS IS'' AND ANY
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>

#include "utility.h"
#include "i2c_blinkm.h"
#include "blinkm_regs.h"
#include "i2c_functions.h"


void read_error(uint8_t led);
void write_error(uint8_t led);

int blinkm_get_address(uint8_t led)
{
	int fh, result;
	uint8_t data;

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data = GET_BLINKM_ADDRESS;

	result = write(fh, &data, 1);

	if (result == 1) {
		data = 0;
		result = read(fh, &data, 1);

		if (result != 1) {
			read_error(led);
			result = -1;
		}	
	} else {
		write_error(led);
		result = -1;
	}

	i2c_end_transaction(fh);

	return result;
}

/*
 * Make sure you only have one BlinkM connected when you run this.
 * Right now this hangs the i2c bus (SDA stays low) and you have to cycle 
 * power for both the overo and the led to get the bus working again. 
 * It does change the address though.
 */
int blinkm_set_address(uint8_t new_addr)
{
	int fh, result;
	uint8_t data[8];

	fh = i2c_start_transaction(0x00);

	if (fh < 0) 
		return -1;

	data[0] = SET_BLINKM_ADDRESS;
	data[1] = new_addr;
	data[2] = 0xd0;
	data[3] = 0x0d;
	data[4] = new_addr;

	result = write(fh, &data, 5);

	if (result == 5) {
		fprintf(stdout, "Set new blinkm address to 0x%02X\n", new_addr);
		result = new_addr;
	} else {
		write_error(0x00);
		result = -1;
	}

	i2c_end_transaction(fh);

	msleep(100);

	return result;
}

int blinkm_set_rgb_color_now(uint8_t led, uint8_t r, uint8_t g, uint8_t b)
{
	int fh, result;
	uint8_t data[4];

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = SET_RGB_COLOR_NOW;
	data[1] = r;
	data[2] = g;
	data[3] = b;

	result = write(fh, data, 4);

	if (result != 4) {
		write_error(led);
		result = -1;
	}

	i2c_end_transaction(fh);

	return result;
}

int blinkm_fade_to_rgb_color(uint8_t led, uint8_t r, uint8_t g, uint8_t b)
{
	int fh, result;
	uint8_t data[4];

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = FADE_TO_RGB_COLOR;
	data[1] = r;
	data[2] = g;
	data[3] = b;

	result = write(fh, data, 4);

	if (result != 4) {
		write_error(led);
		result = -1;
	}

	i2c_end_transaction(fh);

	return result;
}

int blinkm_fade_to_hsb_color(uint8_t led, uint8_t h, uint8_t s, uint8_t b)
{
	int fh, result;
	uint8_t data[4];

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = FADE_TO_HSB_COLOR;
	data[1] = h;
	data[2] = s;
	data[3] = b;

	result = write(fh, data, 4);

	if (result != 4) {
		write_error(led);
		result = -1;
	}

	i2c_end_transaction(fh);

	return result;
}

int blinkm_fade_to_random_rgb_color(uint8_t led, uint8_t r, uint8_t g, uint8_t b)
{
	int fh, result;
	uint8_t data[4];

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = FADE_TO_RANDOM_RGB_COLOR;
	data[1] = r;
	data[2] = g;
	data[3] = b;

	result = write(fh, data, 4);

	if (result != 4) {
		write_error(led);
		result = -1;
	}

	i2c_end_transaction(fh);

	return result;
}

int blinkm_fade_to_random_hsb_color(uint8_t led, uint8_t h, uint8_t s, uint8_t b)
{
	int fh, result;
	uint8_t data[4];

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = FADE_TO_RANDOM_HSB_COLOR;
	data[1] = h;
	data[2] = s;
	data[3] = b;

	result = write(fh, data, 4);

	if (result != 4) {
		write_error(led);
		result = -1;
	}

	i2c_end_transaction(fh);

	return result;
}

int blinkm_get_current_rgb_color(uint8_t led)
{
	int fh, result;
	uint8_t data[4];

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = GET_CURRENT_RGB_COLOR;

	result = write(fh, data, 1);

	if (result == 1) {
		bzero(data, sizeof(data));
		
		result = read(fh, data, 3);

		if (result == 3) {
			/* pack the rgb values into the low three bytes of result */
			result = (data[0] << 16) + (data[1] << 8) + data[2];
		} else {
			read_error(led);
			result = -1;
		}
	} else {
		write_error(led);
		result = -1;
	}

	i2c_end_transaction(fh);

	return result;
}

int blinkm_stop_script(uint8_t led)
{
	int fh, result;
	uint8_t data;

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data = STOP_SCRIPT;

	result = write(fh, &data, 1);

	i2c_end_transaction(fh);

	if (result != 1) {
		write_error(led);
		result = -1;
	}

	return result;
}

int blinkm_play_script(uint8_t led, uint8_t script_id, uint8_t num_repeats)
{
	int fh, result;
	uint8_t data[4];

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = PLAY_LIGHT_SCRIPT;
	data[1] = script_id;
	data[2] = num_repeats;
	/* always starting scripts from line zero for now */
	data[3] = 0;

	result = write(fh, data, 4);

	i2c_end_transaction(fh);

	if (result != 4) {
		write_error(led);
		result = -1;
	}

	return result;
}

int blinkm_set_fade_speed(uint8_t led, uint8_t speed)
{
	int fh, result;
	uint8_t data[2];

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = SET_FADE_SPEED;
	data[1] = speed;

	result = write(fh, data, 2);

	i2c_end_transaction(fh);

	if (result != 2) {
		write_error(led);
		result = -1;
	}

	return result;
}

int blinkm_set_time_adjust(uint8_t led, int8_t adjust)
{
	int fh, result;
	uint8_t data[2];

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = SET_TIME_ADJUST;
	data[1] = adjust;

	result = write(fh, data, 2);

	i2c_end_transaction(fh);

	if (result != 2) {
		write_error(led);
		result = -1;
	}

	return result;
}

int blinkm_read_script_line(uint8_t led, uint8_t line_no, struct script_line *s)
{
	int fh, result;
	uint8_t data[8];

	if (!s) 
		return -1;

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = READ_SCRIPT_LINE;
	/* 
	 *  The blinkm doesn't bring the SDA line high again for any script number but zero. 
	 *  Script zero is the only script you can change so maybe this is by design, not 
	 *  very graceful though.
	 */
	data[1] = 0x00; 
	data[2] = line_no;

	result = write(fh, data, 3);

	if (result != 3) {
		write_error(led);
		result = -1;
	} else {
		bzero(data, sizeof(data));

		result = read(fh, data, 5);

		if (result != 5) {
			read_error(led);
			result = -1;
		} else {
			s->_ticks = data[0];
			s->_cmd = data[1];
			s->_arg[0] = data[2];
			s->_arg[1] = data[3];
			s->_arg[2] = data[4];
		}
	}

	i2c_end_transaction(fh);

	return result;
}

int blinkm_write_script_line(uint8_t led, uint8_t line_no, struct script_line *s)
{
	int fh, result;
	uint8_t data[8];

	if (!s) 
		return -1;

	if (line_no > 49) {
		fprintf(stderr, "Invalid script line number %d\n", line_no);
		return -1;
	}

	switch (s->_cmd) {
	case SET_RGB_COLOR_NOW:
	case FADE_TO_RGB_COLOR:
	case FADE_TO_HSB_COLOR:
	case FADE_TO_RANDOM_RGB_COLOR:
	case FADE_TO_RANDOM_HSB_COLOR:
	case SET_FADE_SPEED:
	case SET_TIME_ADJUST:
		break;

	default:
		fprintf(stderr, "Invalid script command 0x%02X\n", s->_cmd);
		return -1;
	}

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = WRITE_SCRIPT_LINE;
	data[1] = 0x00; 
	data[2] = line_no;
	data[3] = s->_ticks;
	data[4] = s->_cmd;
	data[5] = s->_arg[0];
	data[6] = s->_arg[1];
	data[7] = s->_arg[2];

#if 1 
	result = write(fh, data, 8);

	if (result != 8) {
		write_error(led);
		result = -1;
	}

#else
	fprintf(stderr, "\nWriteScriptLine: ");

	for (i = 0; i < 8; i++) 
		fprintf(stderr, "0x%02X ", data[i]);
	
	fprintf(stderr, "\n");
#endif

	msleep(100);

	i2c_end_transaction(fh);

	return result;
}

int blinkm_set_script_length_and_repeats(uint8_t led, uint8_t length, uint8_t repeats)
{
	int fh, result;
	uint8_t data[4];

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = SET_SCRIPT_LENGTH_AND_REPEATS;
	data[1] = length;
	data[2] = repeats;

	result = write(fh, data, 3);
	
	if (result != 3) {
		write_error(led);
		result = -1;
	}

	msleep(50);

	i2c_end_transaction(fh);

	return result;
}

/*
 * The firmware version is either 'a'.'a' or 'a'.'b' for a BlinkM or MaxM 
 * respectively.
 * They get packed into the low order bytes of the result. 
 * We use this to scan a bus for blinkm devices. The verbose flag will suppress
 * error messages if false.
 */
int blinkm_get_firmware_version(uint8_t led, int verbose)
{
	int fh, result;
	uint8_t data[2];

	fh = i2c_start_transaction(led);

	if (fh < 0) 
		return -1;

	data[0] = GET_FIRMWARE_VERSION;

	result = write(fh, data, 1);

	if (result == 1) {
		data[0] = 0;
		data[1] = 0;

		result = read(fh, data, 2);

		if (result != 2) {
			if (verbose) 
				read_error(led);

			result = -1;
		} else {
			result = data[0];
			result <<= 8;
			result |= data[1];
		}
	} else {
		if (verbose) 
			write_error(led);

		result = -1;
	}

	i2c_end_transaction(fh);

	return result;
}

void read_error(uint8_t led)
{
	fprintf(stderr, "Read failed for device 0x%02X: %s\n", led, strerror(errno));
}

void write_error(uint8_t led)
{
	fprintf(stderr, "Write failed for device 0x%02X: %s\n", led, strerror(errno));
}

