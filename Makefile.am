AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4
GSOAP_SRC_DIR ?= $(top_srcdir)/gsoap-2.8
DEPDIR= .deps

lib_LTLIBRARIES = libonvifdisco.la
libonvifdisco_la_SOURCES = src/discoverer.c src/discoverer.h src/onvif_discovery.c src/onvif_discovery.h $(GSOAP_SRC_DIR)/gsoap/dom.c $(GSOAP_SRC_DIR)/gsoap/stdsoap2.c $(GSOAP_SRC_DIR)/gsoap/plugin/wsaapi.c $(GSOAP_SRC_DIR)/gsoap/plugin/wsddapi.c src/generated/soapClient.c src/generated/soapC.c
libonvifdisco_la_CFLAGS= -c -Wall -fpic -fvisibility=hidden `PKG_CONFIG_PATH=$(GSOAP_SRC_DIR)/build/lib/pkgconfig pkg-config --cflags gsoapssl glib-2.0 gtk+-3.0` -I$(GSOAP_SRC_DIR)/ -I$(GSOAP_SRC_DIR)/build/include/ -I$(GSOAP_SRC_DIR)/gsoap/import -I$(GSOAP_SRC_DIR)/gsoap/custom -I$(GSOAP_SRC_DIR)/gsoap/plugin -I$(srcdir)/src/generated
libonvifdisco_la_LDFLAGS= `PKG_CONFIG_PATH=$(GSOAP_SRC_DIR)/build/lib/pkgconfig  pkg-config --libs openssl zlib glib-2.0 gtk+-3.0`
include_HEADERS= src/discoverer.h src/onvif_discovery.h