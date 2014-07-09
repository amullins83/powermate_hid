SUBDIRS = src
bin_PROGRAMS = powermate_client
powermate_client_SOURCES = src/main.c src/powermate_hid.c src/powermate_hid.h

LDADD = -lusb-1.0 -lhidapi-libusb
AM_CPPFLAGS = -I/usr/local/include/libusb-1.0 -I/usr/local/include/hidapi
