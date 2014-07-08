#Powermate HID Interface Test-bed

Made with [Ceedling](http://throwtheswitch.org)!

##What the what?

The [Griffin Powermate](http://store.griffintechnology.com/powermate) is a handy knob/button controller useful for many applications. Griffin provides driver software for all major operating systems, but the hardware is simple enough to write your own drivers for. This project is intended to provide at least one example of a custom driver with a terminal client.

##Permissions

You may need to set your user permissions to allow reading and writing to the Powermate. If your system uses [udev]() to allow user-space device control, you can copy the included file `55-powermate.rules` to `/etc/udev/rules.d/`. This gives all members of the group `input` read and write access to the device.

##Building

###Currently this project only works in Linux

Instead of `make`, this project uses `rake` and a configuration file `project.yml`. This file defines the compiler and linker flags necessary to build and test the library.

This project depends on [libusb](http://libusb.info), and the given configuration expects `libusb.h` to be found at `/usr/local/include/libusb-1.0`, with the appropriate pre-built library object at `/usr/local/lib`. You can specify different locations in the YAML.

This project also depends on [hidapi](http://www.signal11.us/oss/hidapi/), and the given configuration expects `hidapi.h` to be found at `/usr/local/include/hidapi`, with a libusb-enabled pre-built library object at `usr/local/lib`. Again, these locations can be set in the YAML.

With libusb and hidapi installed and the configuration set up to find them, run `rake test:all` in the project folder. 

If you have any problems, let me know.

##Client

You can try out the functions in this library by building the client code in `src/main.c`. On my system, I built with:

```bash
$rake test:all #builds library in build/test/out
$gcc -c src/main.c -o build/main.o
$gcc -o bin/powermate_client build/main.o build/test/out/powermate_hid.o -lusb-1.0 -lhidapi-libusb
```
If you feel like your system is probably similar to mine, you can take a chance on the included `build_powermate_client.sh` script, which will run the above commands for you.

Once built, you can run `$bin/powermate_client` to experiment with an interactive command line. A more practical use of his client is to send a small set of commands as a single command line argument, which will run each command in order.

###The commands:

    h:                   Show this help
    l:                      Turn LED on
    m:   Set LED to half power (medium)
    L:                     Turn LED off
    i:              Get an input report
    p:                Enable pulse mode
    P:               Disable pulse mode
    f:                       Fast pulse
    s:                       Slow pulse
    q:                             Quit

###Examples

    $bin/powermate_client pf    # Pulse fast
    $bin/powermate_client ps    # Pulse slow
    $bin/powermate_client PL    # Turn off
    $bin/powermate_client i     # Print one input report
