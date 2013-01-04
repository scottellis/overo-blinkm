  Overview
--------

A command line program for controlling BlinkM leds connected 
to the I2C bus of a embedded Linux machine. Typically only
embedded Linux systems export /dev/i2c busses to userland.

The default assumes a Gumstix Overo board.

If you are using an RPi, then change the /dev/i2c-X constant in 
i2c_functions.c to use the appropriate i2c bus for your board.


  Building
--------

There is a make file for building the project.

        $ git pull git://github.com/scottellis/overo-blinkm.git
        $ cd overo-blinkm
        $ <optional> edit i2c_functions.c
        $ make


  Running
--------

You will need to change permissions for /dev/i2c-3 if you
are running the program as someone other then root.

A one-time fix is to run the following as root

        chmod 0666 /dev/i2c-3

udev will overwrite this on your next boot though.

A more permanent fix is to create a udev rule file like this 

        /etc/udev/rules.d/90-i2c.rules

With the following single line

        KERNEL=="i2c-[0-3]", GROUP="i2c", MODE="0666"


Run the blinkm program without arguments to get the available commands.

        $ ./blinkm

        Usage: blinkm <command> <args>

        The led address is optional and defaults to 0x09.
        Use a comma separated list to address multiple devices in one command.
        The color arguments are optional and default to zero.

        Available Commands
                find-leds 
                set-rgb [-d led] [-r red] [-g green] [-b blue]
                get-rgb [-d led]
                fade-rgb [-d led] [-r red] [-g green] [-b blue]
                fade-hsb [-d led] [-h hue] [-s saturation] [-b brightness]
                fade-random-rgb [-d led] [-r red] [-g green] [-b blue]
                fade-random-hsb [-d led] [-h hue] [-s saturation] [-b brightness]
                play-script [-d led] -s <script id or name> -n num_repeats
                stop-script [-d led]
                set-fade-speed [-d led] -f speed
                set-time-adjust [-d led] -t adjust
                show-scripts 
                read-script [-d led]
                write-script-line [-d led] -n line_no -t ticks -c cmd -a arg1[,arg2[,arg3]]
                set-script-length-and-repeats [-d led] -l length -n repeats
                set-address -d new_led_address


The first command you probably want to run is find-leds.

        $ ./blinkm find-leds

        Scanning I2C bus for BlinkM devices...
        Found a BlinkM at address 1 (0x01)
        Found a BlinkM at address 2 (0x02)
        Found a BlinkM at address 3 (0x03)
        Found a BlinkM at address 4 (0x04)
        Found 4 devices



  TODO
--------

1. Write script function hasn't been tested much.

2. Need to recover the bus after the command to change an led address.
   Right now you need to power cycle the led which is okay, but you
   also need to power cycle the overo or the i2c bus stays hung.
   
3. GetAddress is not implemented. Investigate if i2c broadcasts are
   possible with the overos. 

4. Add code to read in scripts from a file.
