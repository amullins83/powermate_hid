#Powermate HID Interface Test-bed

Made with [Ceedling](http://throwtheswitch.org)!

##What the what?

The [Griffin Powermate](http://store.griffintechnology.com/powermate) is a handy knob/button controller useful for many applications. Griffin provides driver software for all major operating systems, but the hardware is simple enough to write your own drivers for. This project is intended to provide at least one example of a custom driver with a terminal client.

##Permissions

You may need to set your user permissions to allow reading and writing to the Powermate. If your system uses [udev]() to allow user-space device control, you can copy the included file `55-powermate.rules` to `/etc/udev/rules.d/`. This gives all members of the group `input` read and write access to the device.

##Building

###Currently this project only works in Linux

This project uses `rake` and a configuration file `project.yml` to run tests. This file defines the compiler and linker flags necessary to build and test the library.

This project depends on [libusb](http://libusb.info), and the given configuration expects `libusb.h` to be found at `/usr/local/include/libusb-1.0`, with the appropriate pre-built library object at `/usr/local/lib`. You can specify different locations in the YAML.

This project also depends on [hidapi](http://www.signal11.us/oss/hidapi/), and the given configuration expects `hidapi.h` to be found at `/usr/local/include/hidapi`, with a libusb-enabled pre-built library object at `usr/local/lib`. Again, these locations can be set in the YAML.

With libusb and hidapi installed and the configuration set up to find them, run `rake test:all` in the project folder. 

The tests will only pass if 

 1. A Griffin Powermate is connected to your system **AND**
 2. Your user has read and write permissions for it

If you have any problems, let me know.

##Client

You can try out the functions in this library by building and running the included sample client. Just build the library the normal way:

    $./configure && make

If all goes well, you should have an executable `./powermate_client` in the project directory. Running it with no arguments starts an interactive session. Running with a set of command characters will run the respective commands and then stop. There is no delay between commands, so this won't allow custom LED effects with a single execution.

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

    $./powermate_client pf    # Pulse fast
    $./powermate_client ps    # Pulse slow
    $./powermate_client PL    # Turn off
    $./powermate_client i     # Print one input report
