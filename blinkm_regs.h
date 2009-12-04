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

#ifndef BLINKM_REGS_H
#define BLINKM_REGS_H

/* FIND_LEDS is not a real blinkm command, but a command to scan the bus for leds */
#define FIND_LEDS 0x33

/* BlinkM leds have firmware version 'a'.'a' */
#define BLINKM_DEVICE_FIRMWARE 0x6161
/* MaxM devices have firmware version 'a'.'b' */
/* Haven't actually verified this */
#define MAXM_DEVICE_FIRMWARE 0x6162


/* 3 args, R, G, B */
#define SET_RGB_COLOR_NOW				0x6E

/* 3 args, R, G, B */
#define FADE_TO_RGB_COLOR				0x63

/* 3 args, H, S, B */
#define FADE_TO_HSB_COLOR				0x68

/* 3 args, R, G, B */
#define FADE_TO_RANDOM_RGB_COLOR		0x43

/* 3 args, H, S, B */
#define FADE_TO_RANDOM_HSB_COLOR		0x48

#define PLAY_LIGHT_SCRIPT				0x70
#define STOP_SCRIPT						0x6F
#define SET_FADE_SPEED					0x66
#define SET_TIME_ADJUST					0x74

/* 3 return bytes */
#define GET_CURRENT_RGB_COLOR			0x67

#define WRITE_SCRIPT_LINE				0x57
#define READ_SCRIPT_LINE				0x52
#define SET_SCRIPT_LENGTH_AND_REPEATS	0x4C

/* 4 args, <new address>, 0xd0, 0x0d, <new address> */
#define SET_BLINKM_ADDRESS				0x41

/* 1 return byte */
#define GET_BLINKM_ADDRESS				0x61

/* 1 return byte */
#define GET_FIRMWARE_VERSION			0x5A

/* 5 args */
#define SET_STARTUP_PARAMETERS			0x42

#endif
