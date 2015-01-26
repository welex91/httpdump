AM_CPPFLAGS = $(apr_CFLAGS)
bin_PROGRAMS = httpdump test_parser
httpdump_SOURCES = httpdump.c parser.c json_format.c xml_format.c
httpdump_LDADD = $(apr_LIBS) $(PCAP_LIBS)
httpdump_CPPFLAGS = -I../include $(apr_CFLAGS)

test_parser_SOURCES = test_parser.c parser.c
test_parser_CPPFLAGS = -I../include 