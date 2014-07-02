#!/bin/bash

rake test:all
gcc -o build/test/out/main.o -c src/main.c -Isrc
gcc -o bin/powermate_client build/test/out/main.o \
       build/test/out/powermate_hid.o -lhidapi-libusb -lusb-1.0
