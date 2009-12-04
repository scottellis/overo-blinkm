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

#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdint.h> 
#include <ctype.h>

#include "i2c_blinkm.h"
#include "blinkm_regs.h"

struct cmd {
	char _cmd[32];
	char _args[64];
};

#define CMD_SHOW_USAGE 0
#define CMD_FIND_LEDS 1 
#define CMD_SET_RGB 2 
#define CMD_GET_RGB 3 
#define CMD_FADE_RGB 4 
#define CMD_FADE_HSB 5 
#define CMD_FADE_RANDOM_RGB 6 
#define CMD_FADE_RANDOM_HSB 7 
#define CMD_PLAY_SCRIPT 8 
#define CMD_STOP_SCRIPT 9 
#define CMD_SET_FADE_SPEED 10 
#define CMD_SET_TIME_ADJUST 11
#define CMD_SHOW_SCRIPTS 12 
#define CMD_READ_SCRIPT 13 
#define CMD_WRITE_SCRIPT_LINE 14
#define CMD_SET_SCRIPT_LENGTH_AND_REPEATS 15
#define CMD_SET_ADDRESS 16 
#define NUM_COMMANDS 17

struct cmd commands[NUM_COMMANDS] = {
	{ "usage", "" },
	{ "find-leds", "" },
	{ "set-rgb", "[-d led] [-r red] [-g green] [-b blue]" },
	{ "get-rgb", "[-d led]" },
	{ "fade-rgb", "[-d led] [-r red] [-g green] [-b blue]" },
	{ "fade-hsb", "[-d led] [-h hue] [-s saturation] [-b brightness]" },
	{ "fade-random-rgb", "[-d led] [-r red] [-g green] [-b blue]" },
	{ "fade-random-hsb", "[-d led] [-h hue] [-s saturation] [-b brightness]" },
	{ "play-script", "[-d led] -s <script id or name> -n num_repeats" },
	{ "stop-script", "[-d led]" },
	{ "set-fade-speed", "[-d led] -f speed" },
	{ "set-time-adjust", "[-d led] -t adjust" },
	{ "show-scripts", "" },
	{ "read-script", "[-d led]" },
	{ "write-script-line", "[-d led] -n line_no -t ticks -c cmd -a arg1[,arg2[,arg3]]" },
	{ "set-script-length-and-repeats", "[-d led] -l length -n repeats" },
	{ "set-address", "-d new_led_address" }
};


struct script {
	char _name[32];
	char _description[64];
};

#define MAX_SCRIPTS 19
struct script scripts[MAX_SCRIPTS] = {
	{ "default", "white > red > green > blue > off" },
	{ "rgb", "red > green > blue" },
	{ "white-flash", "white > off" },
	{ "red-flash", "red > off" },
	{ "green-flash", "green > off" },
	{ "blue-flash", "blue > off" },
	{ "cyan-flash", "cyan > off" },
	{ "magenta-flash", "magenta > off" },
	{ "yellow-flash", "yellow > off" },
	{ "black", "off" },
	{ "hue-cycle", "red > yellow > green > cyan > blue > purple" },
	{ "mood-light", "random hue > random hue" },
	{ "virtual-candle", "random yellows" },
	{ "water-reflections", "random blues" },
	{ "old-neon", "random orangish reds" },
	{ "the-seasons", "spring colors > summer > fall > winter" },
	{ "thunderstorm", "random blues & purples > white flashes" },
	{ "stop-light", "red > green > yellow" },
	{ "morse-code", "S.O.S. in white" }
};

#define MAX_LEDS_PER_CMD 32 

struct blinkm_args {
	int _cmd;
	int _num_leds;
	int _led[MAX_LEDS_PER_CMD];
	int _red;
	int _green;
	int _blue;
	int _hue;
	int _saturation;
	int _brightness;
	int _script_id;
	int _num_repeats;
	int _fade_speed;
	int _time_adjust;
	int _line_no;
	struct script_line _script_line;
};

int parse_args(int argc, char **argv, struct blinkm_args *ba);
int get_led_arg(char *arg, struct blinkm_args *ba);
int get_script_arg(char *arg);
int check_args(struct blinkm_args *ba);
void run_led_command(struct blinkm_args *ba, int led_index);
void run_command(struct blinkm_args *ba);
void scan_bus_for_leds();
void read_script(uint8_t led_addr);
int get_write_script_line_cmd(char *arg);
int get_write_script_line_cmd_args(char *arg, struct blinkm_args *ba);


int main(int argc, char **argv)
{
	struct blinkm_args ba;

	if (!parse_args(argc, argv, &ba)) 
		ba._cmd = CMD_SHOW_USAGE;
	else if (!check_args(&ba)) 
		ba._cmd = CMD_SHOW_USAGE;
	
	run_command(&ba);

	return 0;
}

int parse_args(int argc, char **argv, struct blinkm_args *ba)
{
	int opt, i;
	char *end;

	if (argc < 2) 
		return 0;

	bzero(ba, sizeof(struct blinkm_args));
	ba->_script_id = -1;

	while ((opt = getopt(argc, argv, "d:r:g:b:s:h:n:f:t:c:a:")) != -1) {
	
		switch (opt) {
		case 'd':
			ba->_num_leds = get_led_arg(optarg, ba);
			break;

		case 'r':
			ba->_red = strtol(optarg, &end, 0);
			break;

		case 'g':
			ba->_green = strtol(optarg, &end, 0);
			break;

		case 'b':
			ba->_blue = strtol(optarg, &end, 0);
			ba->_brightness = ba->_blue;
			break;

		case 's':
			/* this also works to get the saturation color */
			ba->_script_id = get_script_arg(optarg);
			ba->_saturation = ba->_script_id;
			break;

		case 'h':
			ba->_hue = strtol(optarg, &end, 0);
			break;

		case 'n':
			ba->_num_repeats = strtol(optarg, &end, 0);
			ba->_line_no = ba->_num_repeats;
			break;

		case 'f':
			ba->_fade_speed = strtol(optarg, &end, 0);
			break;

		case 't':
			ba->_time_adjust = strtol(optarg, &end, 0);
			ba->_script_line._ticks = ba->_time_adjust;
			break;

		case 'c':
			ba->_script_line._cmd = get_write_script_line_cmd(optarg);
			break;

		case 'a':
			get_write_script_line_cmd_args(optarg, ba);
			break;
		}
	}

	if (optind < argc) 
		for (i = 1; i < NUM_COMMANDS; i++) 
			if (!strcasecmp(argv[optind], commands[i]._cmd)) {
				ba->_cmd = i;
				break;
			}
		
	return 1;
}

/*
 * The -d arg can take a comma separated list of up to MAX_LEDS_PER_CMD numbers. 
 */
int get_led_arg(char *arg, struct blinkm_args *ba)
{
	char buff[64];
	char *p, *end;
	int i;
	
	if (strlen(arg) > sizeof(buff) - 1) {
		printf("Unreasonably long list for the led argument: %s\n", arg);
		return 0;
	}

	strcpy(buff, arg);

	i = 0;
	p = strtok(buff, ",");

	while (p && i < MAX_LEDS_PER_CMD) {
		ba->_led[i] = strtol(p, &end, 0);
	
		/* skip the bad ones */
		if (ba->_led[i] < 1 ||  ba->_led[i] > 127) {
			printf("Invalid led address %d ignored\n", ba->_led[i]);
			ba->_led[i] = 0;
		}
		else {
			i++;
		}

		p = strtok(NULL, ",");
	}
	
	return i;		
}

/*
 * The -s arg can take a script number or a name
 */
int get_script_arg(char *arg)
{
	int i, script_no;
	char *end;

	if (isdigit(arg[0])) {
		script_no = strtol(arg, &end, 0);
	}
	else {
		script_no = 0;

		for (i = 0; i < MAX_SCRIPTS; i++) 
			if (!strcasecmp(scripts[i]._name, arg)) {
				script_no = i;
				break;
			}
	}

	return script_no;
}

int get_write_script_line_cmd(char *arg)
{
	int i, cmd;

	for (i = 1; i < NUM_COMMANDS; i++) 
		if (!strcasecmp(arg, commands[i]._cmd)) 
			break;
	

	switch (i) {
	case CMD_SET_RGB:
		cmd = SET_RGB_COLOR_NOW;
		break;

	case CMD_FADE_RGB:
		cmd = FADE_TO_RGB_COLOR;
		break;

	case CMD_FADE_HSB:
		cmd = FADE_TO_HSB_COLOR;
		break;

	case CMD_FADE_RANDOM_RGB:
		cmd = FADE_TO_RANDOM_RGB_COLOR;
		break;

	case CMD_FADE_RANDOM_HSB:
		cmd = FADE_TO_RANDOM_HSB_COLOR;
		break;

	case CMD_SET_FADE_SPEED:
		cmd = SET_FADE_SPEED;
		break;

	case CMD_SET_TIME_ADJUST:
		cmd = SET_TIME_ADJUST;
		break;

	default:
		cmd = 0;
	}

	return cmd;
}

int get_write_script_line_cmd_args(char *arg, struct blinkm_args *ba)
{
	char buff[64];
	char *p, *end;
	int i, val;
	
	if (strlen(arg) > sizeof(buff) - 1) {
		printf("Unreasonably long list for the script line arguments: %s\n", arg);
		return 0;
	}

	strcpy(buff, arg);

	i = 0;
	p = strtok(buff, ",");

	while (p && i < 3) {
		val = strtol(p, &end, 0);

		if (val < 0 || val > 255) 
			break;

		ba->_script_line._arg[i] = val;
		i++;
		p = strtok(NULL, ",");
	}
	
	return i;		
}

int check_args(struct blinkm_args *ba)
{
	int need_led = 0;
	int result = 1;

	switch (ba->_cmd) {
	case CMD_SET_RGB:
	case CMD_FADE_RGB:
	case CMD_FADE_RANDOM_RGB:
		need_led = 1;

		if (ba->_red < 0 || ba->_red > 255) {
			result = 0;
			printf("Red range is 0-255\n");
		}
		else if (ba->_green < 0 || ba->_green > 255) {
			result = 0;
			printf("Green range is 0-255\n");
		}
		else if (ba->_blue < 0 || ba->_blue > 255) {
			result = 0;
			printf("Blue range is 0-255\n");
		}

		break;

	case CMD_FADE_HSB:
	case CMD_FADE_RANDOM_HSB:
		need_led = 1;

		if (ba->_hue < 0 || ba->_hue > 255) {
			result = 0;
			printf("Hue range is 0-255\n");
		}
		else if (ba->_saturation < 0 || ba->_saturation > 255) {
			result = 0;
			printf("Saturation range is 0-255\n");
		}
		else if (ba->_brightness < 0 || ba->_brightness > 255) {
			result = 0;
			printf("Brightness range is 0-255\n");
		}

		break;

	case CMD_PLAY_SCRIPT:
		need_led = 1;

		if (ba->_script_id < 0 || ba->_script_id >= MAX_SCRIPTS) {
			result = 0;
			printf("Script id range is 0-%d\n", MAX_SCRIPTS - 1);
		}

		if (ba->_num_repeats < 0 || ba->_num_repeats > 255) {
			result = 0;
			printf("Script repeat range is 0-255. The default of zero plays the script forever.\n");
		}

		break;

	case CMD_SET_FADE_SPEED:
		need_led = 1;

		if (ba->_fade_speed < 0 || ba->_fade_speed > 255) {
			result = 0;
			printf("Fade speed range is 1-255\n");
		}

		break;

	case CMD_SET_TIME_ADJUST:
		need_led = 1;

		if (ba->_time_adjust < -128 || ba->_time_adjust > 127) {
			result = 0;
			printf("Time adjust range is -128 to 127. Zero resets playback speed to the default.\n");
		}

		break;

	case CMD_WRITE_SCRIPT_LINE:
		need_led = 1;

		if (ba->_line_no < 0 || ba->_line_no > 49) {
			result = 0;
			fprintf(stderr, "Valid script line numbers are 0-49\n");
		}
		else if (ba->_script_line._cmd == 0) {
			result = 0;
			fprintf(stderr, "Invalid write script command\n");
		}
		else if (ba->_script_line._ticks == 0) {
			ba->_script_line._ticks = 50;
		}

		break;

	case CMD_GET_RGB:
	case CMD_STOP_SCRIPT:
	case CMD_READ_SCRIPT:
	case CMD_SET_ADDRESS:
		need_led = 1;
		break;

	/* these commands don't require any arguments */
	case CMD_FIND_LEDS:
	case CMD_SHOW_SCRIPTS:
	case CMD_SHOW_USAGE:
		break;

	default:
		result = 0;
	}

	if (result && need_led && ba->_num_leds == 0) {
		ba->_led[0] = DEFAULT_BLINKM_I2C_ADDRESS;
		ba->_num_leds = 1;
	}

	return result;
}

void run_led_command(struct blinkm_args *ba, int led_index)
{
	int rgb;

	switch (ba->_cmd) {
	case CMD_SET_RGB:
		blinkm_set_rgb_color_now(ba->_led[led_index], ba->_red, ba->_green, ba->_blue);
		break;

	case CMD_GET_RGB:
		rgb = blinkm_get_current_rgb_color(ba->_led[led_index]);

		if (rgb > 0) 
			printf("Led %d rgb(%d, %d, %d)\t[Led 0x%02x (0x%02x, 0x%02x, 0x%02x)]\n",
					ba->_led[led_index], (rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff,
					ba->_led[led_index], (rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);

		break;

	case CMD_FADE_RGB:
		blinkm_fade_to_rgb_color(ba->_led[led_index], ba->_red, ba->_green, ba->_blue);
		break;

	case CMD_FADE_HSB:
		blinkm_fade_to_hsb_color(ba->_led[led_index], ba->_hue, ba->_saturation, ba->_brightness);
		break;

	case CMD_FADE_RANDOM_RGB:
		blinkm_fade_to_random_rgb_color(ba->_led[led_index], ba->_red, ba->_green, ba->_blue);
		break;

	case CMD_FADE_RANDOM_HSB:
		blinkm_fade_to_random_hsb_color(ba->_led[led_index], ba->_hue, ba->_saturation, ba->_brightness);
		break;

	case CMD_PLAY_SCRIPT:
		blinkm_play_script(ba->_led[led_index], ba->_script_id, ba->_num_repeats);
		break;

	case CMD_STOP_SCRIPT:
		blinkm_stop_script(ba->_led[led_index]);
		break;

	case CMD_SET_FADE_SPEED:
		blinkm_set_fade_speed(ba->_led[led_index], ba->_fade_speed);
		break;

	case CMD_SET_TIME_ADJUST:
		blinkm_set_time_adjust(ba->_led[led_index], (int8_t) ba->_time_adjust);
		break;

	case CMD_SET_ADDRESS:
		blinkm_set_address(ba->_led[led_index]);
		break;

	case CMD_READ_SCRIPT:
		/* 
		 * Have to stop the script first or the leds sometimes stop talking
		 *  and hangs the whole i2c bus, i.e. sda never comes high again 
	         */
		if (blinkm_stop_script(ba->_led[led_index]) > 0) 
			read_script(ba->_led[led_index]);
		
		break;

	case CMD_WRITE_SCRIPT_LINE:
		blinkm_write_script_line(ba->_led[led_index], ba->_line_no, &ba->_script_line);
		break;
	}
}

void run_command(struct blinkm_args *ba) 
{
	int i;

	switch (ba->_cmd) {
	case CMD_FIND_LEDS:
		scan_bus_for_leds();
		break;

	case CMD_SHOW_SCRIPTS:
		printf("\nAvailable Scripts\n\nId                  Name        Sequence\n");

		for (i = 0; i < MAX_SCRIPTS; i++) 
			printf("%2d  %20s\t%s\n", i, scripts[i]._name, scripts[i]._description);

		printf("\n");
		break;

	case CMD_SHOW_USAGE:
		printf("\nUsage: blinkm <command> <args>\n\n"
			"The led address is optional and defaults to 0x09.\n"
			"Use a comma separated list to address multiple devices in one command.\n"
			"The color arguments are optional and default to zero.\n"
			"\nAvailable Commands\n");

		for (i = 1; i < NUM_COMMANDS; i++) 
			printf("\t%s %s\n", commands[i]._cmd, commands[i]._args);

		printf("\n");
		break;

	default:
		for (i = 0; i < ba->_num_leds; i++) 
			run_led_command(ba, i);

		break;
	}
}

/*
   ================================================================================================
   ================================================================================================
*/
void read_script(uint8_t led)
{
	int i;
	struct script_line sline;

	for (i = 0; i < MAX_SCRIPT_LINES; i++) { 
		bzero(&sline, sizeof(sline));

		if (blinkm_read_script_line(led, i, &sline) < 0) 
			break;

		if (sline._ticks == 255 && sline._cmd == 255) 
			break;
	
		if (sline._ticks == 0 && sline._cmd == 0) 
			break;
		

		printf("{ W, 0, %d, %d, %c, 0x%02X, 0x%02X, 0x%02X }\tt=%3d  ",
				i, sline._ticks, sline._cmd, sline._arg[0], 
				sline._arg[1], sline._arg[2], sline._ticks);

		switch (sline._cmd) {
		case FADE_TO_RANDOM_RGB_COLOR:
			printf("FadeToRandomRGBColor(%d, %d, %d)\n",
					sline._arg[0], sline._arg[1], sline._arg[2]);
			break;

		case FADE_TO_RANDOM_HSB_COLOR:
			printf("FadeToRandomHSBColor(%d, %d, %d)\n",
					sline._arg[0], sline._arg[1], sline._arg[2]);
			break;

		case FADE_TO_RGB_COLOR:
			printf("FadeToRGBColor(%d, %d, %d)\n",
					sline._arg[0], sline._arg[1], sline._arg[2]);
			break;

		case FADE_TO_HSB_COLOR:
			printf("FadeToHSBColor(%d, %d, %d)\n",
					sline._arg[0], sline._arg[1], sline._arg[2]);
			break;

		case SET_RGB_COLOR_NOW:
			printf("SetRGBColorNow(%d, %d, %d)\n",
					sline._arg[0], sline._arg[1], sline._arg[2]);
			break;

		case SET_FADE_SPEED:
			printf("SetFadeSpeed(%d)\n", 
					sline._arg[0]);
			break;

		case SET_TIME_ADJUST:
			printf("SetTimeAdjust(%d)\n", 
					sline._arg[0]);
			break;

		default:
			printf("??? script command: 0x%02X ('%c')\n", 
					sline._cmd, (char) sline._cmd);
			break;
		}
	}
}

/*
 * blinkm_get_firmware_version returns the major.minor packed in an int.
 * 'a'.'a' is a standard BlinkM = 0x6161
 * 'a'.'b' is a MaxM BlinkM = 0x6162
 */
void scan_bus_for_leds()
{
	int count, firmware;
	uint8_t led;

	count = 0;

	printf("\nScanning I2C bus for BlinkM devices...\n");

	for (led = 1; led < 128; led++) {
		firmware = blinkm_get_firmware_version(led, 0);

		if (firmware < 1)
			continue;

		if (BLINKM_DEVICE_FIRMWARE == firmware) 	
			printf("Found a BlinkM at address %d (0x%02X)\n", led, led);
		else if (MAXM_DEVICE_FIRMWARE == firmware) 
			printf("Found a MaxM BlinkM at address %d (0x%02X)\n", led, led);
		else 
			printf("Found an uknown device at address %d (0x%02X) : 0x%04X\n", 
					led, led, firmware);
			
		count++;
		
	}

	if (count == 1) 
		printf("Found 1 device\n\n");
	else 
		printf("Found %d devices\n\n", count);
}

