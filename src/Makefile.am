lib_LIBRARIES = libpowermatehid.a
libpowermatehid_a_SOURCES = powermate_hid.c powermate_hid.h

include_HEADERS = powermate_hid.h

LDADD = -lusb-1.0 -lhidapi-libusb
AM_CPPFLAGS = -I/usr/local/include/libusb-1.0 -I/usr/local/include/hidapi
