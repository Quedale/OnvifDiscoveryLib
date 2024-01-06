pkgconfig_DATA = onvifdisco.pc
AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4
GSOAP_SRC_DIR ?= $(top_srcdir)/gsoap-2.8
DEPDIR= .deps

lib_LTLIBRARIES = libonvifdisco.la
libonvifdisco_la_SOURCES = src/discoverer.c src/discoverer.h src/onvif_discovery.c src/onvif_discovery.h $(GSOAP_SRC_DIR)/gsoap/dom.c $(GSOAP_SRC_DIR)/gsoap/stdsoap2.c $(GSOAP_SRC_DIR)/gsoap/plugin/logging.c $(GSOAP_SRC_DIR)/gsoap/plugin/wsaapi.c $(GSOAP_SRC_DIR)/gsoap/plugin/wsddapi.c src/generated/soapClient.c src/generated/soapC.c src/probmatch.c src/probmatch.h
libonvifdisco_la_CFLAGS= -c -Wall -fvisibility=hidden -DWITH_DOM -DWITH_OPENSSL -DWITH_GZIP `PKG_CONFIG_PATH=$(PKG_FULL_PATH) pkg-config --cflags cutils` -I$(GSOAP_SRC_DIR)/gsoap/  -I$(GSOAP_SRC_DIR)/gsoap/import -I$(GSOAP_SRC_DIR)/gsoap/custom -I$(GSOAP_SRC_DIR)/gsoap/plugin -I$(srcdir)/src/generated
libonvifdisco_la_LDFLAGS= `PKG_CONFIG_PATH=$(PKG_FULL_PATH)  pkg-config --libs cutils`
include_HEADERS= src/discoverer.h src/onvif_discovery.h src/probmatch.h