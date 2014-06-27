#Powermate HID Interface Test-bed

Made with [Ceedling](http://throwtheswitch.org)!

##What the what?

The [Griffin Powermate](http://store.griffintechnology.com/powermate) is a handy knob/button controller useful for many applications. Griffin provides driver software for all major operating systems, but the hardware is simple enough to write your own drivers for. This project is intended to provide at least one example of a custom driver with a terminal client.

##Building

###Currently this project only works in Linux

Instead of `make`, this project uses `rake` and a configuration file `project.yml`. This file defines the compiler and linker flags necessary to build and test the library.

This project depends on [libusb](http://libusb.info), and the given configuration expects `libusb.h` to be found at `/usr/local/include/libusb-1.0`, with the appropriate pre-built library object at `/usr/local/lib`. You can specify different locations in the YAML.

With libusb installed and the configuration set up to find it, run `rake test:all` in the project folder. 

If you have any problems, let me know.

##Client

You can try out the functions in this library by building the client code in `src/main.c`. On my system, I built with:

```bash
$rake test:all #builds library in build/test/out
$clang -c src/main.c -o build/main.o
$clang -o powermate_client build/main.o build/test/out/powermate_hid.o -lusb-1.0
```
