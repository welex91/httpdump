#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([httpdump], [1.1], [sroy@blinkx.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/httpdump.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile man/Makefile httpdump.spec])

# Checks for programs.
AC_PROG_CC
PKG_CHECK_MODULES([apr], [apr-1])

# Checks for libraries.
AC_ARG_WITH([pcap-lib-path], [AS_HELP_STRING([--with-pcap-lib-path], [location of the Pcap libraries.])],
[PCAP_LIBS="-L$withval -lpcap"],
[PCAP_LIBS='-lpcap'])
AC_SUBST([PCAP_LIBS])

# Checks for header files.
AC_ARG_WITH([pcap-include-path],
  [AS_HELP_STRING([--with-pcap-include-path],
    [location of the Pcap headers, defaults to /usr/include/pcap])],
  [PCAP_CFLAGS="-I$withval"],
  [PCAP_CFLAGS='-I/usr/include/pcap'])
AC_SUBST([MYSQL_CFLAGS])
AC_CHECK_HEADERS([apr_hash.h])
AC_CHECK_HEADER([pcap.h])
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit memmove regcomp])

AC_OUTPUT
