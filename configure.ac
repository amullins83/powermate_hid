AC_INIT([powermate_hid], [0.1.0], [austin.mullins@gmail.com])

AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign -Wall -Werror dist-bzip2])

AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR

AC_CHECK_LIB([usb-1.0], [libusb_handle_events_timeout_completed], \
  [LIBUSB=-lusb-1.0], [LIBUSB=no])

if test $LIBUSB = no; then
	AC_MSG_ERROR([Could not find libusb-1.0 in lib dir])
fi

AC_SUBST([LIBUSB])

AC_CHECK_LIB([hidapi-libusb], [hid_init], [LIBHIDAPI=-lhidapi-libusb], \
	[LIBHIDAPI=no])

if test $LIBHIDAPI = no; then
	AC_MSG_ERROR([Could not find libhidapi in lib dir])
fi

AC_SUBST([LIBHIDAPI])

AC_CHECK_HEADER([libusb-1.0/libusb.h], [HEADER_LIBUSB=-Ilibusb-1.0], [HEADER_LIBUSB=no])

if test $HEADER_LIBUSB = no; then
	AC_MSG_ERROR([Could not find libusb.h in include dir])
fi

AC_SUBST([HEADER_LIBUSB])

AC_CHECK_HEADER([hidapi/hidapi.h], [HEADER_HIDAPI=-Ihidapi], [HEADER_HIDAPI=no])

if test $HEADER_HIDAPI = no; then
	AC_MSG_ERROR([Could not find hidapi.h in include dir])
fi

AC_SUBST([HEADER_HIDAPI])

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
