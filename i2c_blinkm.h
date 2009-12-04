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

	THIS SOFTWARE IS PROVIDED BY Scott Ellis ''AS IS'' AND ANY
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL Scott Ellis BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef I2C_BLINKM_H
#define I2C_BLINKM_H

#define DEFAULT_BLINKM_I2C_ADDRESS 0x09

#define MAX_SCRIPT_LINES 50

#ifdef __cplusplus
extern "C" {
#endif

struct script_line {
	uint8_t _ticks;
	uint8_t _cmd;
	uint8_t _arg[3];
};

int blinkm_get_address(uint8_t led);
int blinkm_set_address(uint8_t new_addr);
int blinkm_get_firmware_version(uint8_t led, int verbose);

int blinkm_set_rgb_color_now(uint8_t led, uint8_t r, uint8_t g, uint8_t b);
int blinkm_fade_to_rgb_color(uint8_t led, uint8_t r, uint8_t g, uint8_t b);
int blinkm_fade_to_hsb_color(uint8_t led, uint8_t h, uint8_t s, uint8_t b);
int blinkm_fade_to_random_rgb_color(uint8_t led, uint8_t r, uint8_t g, uint8_t b);
int blinkm_fade_to_random_hsb_color(uint8_t led, uint8_t h, uint8_t s, uint8_t b);

int blinkm_get_current_rgb_color(uint8_t led);

int blinkm_stop_script(uint8_t led);
int blinkm_play_script(uint8_t led, uint8_t script_id, uint8_t num_repeats);
int blinkm_set_fade_speed(uint8_t led, uint8_t speed);
int blinkm_set_time_adjust(uint8_t led, int8_t adjust);

int blinkm_read_script_line(uint8_t led, uint8_t line_no, struct script_line *s);
int blinkm_write_script_line(uint8_t led, uint8_t line_no, struct script_line *s);
int blinkm_set_script_length_and_repeats(uint8_t led, uint8_t length, uint8_t repeats);

#ifdef __cplusplus
}
#endif


#endif 
