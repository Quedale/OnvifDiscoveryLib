AUTOMAKE_OPTIONS = foreign

GSOAP_SRC_DIR ?= $(shell pwd)/gsoap-2.8

udptest$(EXEEXT):
	@echo "Compiling udptest..."
	@mkdir -p build
	@gcc \
		`pkg-config --cflags gtk+-3.0` \
		`pkg-config --cflags openssl` \
		-Lbuild/lib \
		src/udp-test.c \
		-o build/udptest \
			-lonvif_discovery \
			`pkg-config --libs gtk+-3.0` \
			`pkg-config --libs openssl`


discoverylib$(EXEEXT):
	@echo "Compiling Discovery Library..."
	@mkdir -p build
	@mkdir -p build/lib
	@gcc -c -Wall -fpic -O2 -flto \
		`PKG_CONFIG_PATH=$(GSOAP_SRC_DIR)/build/lib/pkgconfig pkg-config --cflags gsoap` \
		`PKG_CONFIG_PATH=$(GSOAP_SRC_DIR)/build/lib/pkgconfig pkg-config --cflags gsoapssl` \
		`pkg-config --cflags openssl` \
		`pkg-config --cflags zlib` \
        `pkg-config --cflags gtk+-3.0` \
        `pkg-config --cflags glib-2.0` \
		-I$(GSOAP_SRC_DIR)/ \
		-I$(GSOAP_SRC_DIR)/build/include/ \
		-I$(GSOAP_SRC_DIR)/build/share/gsoap/import \
		-I$(GSOAP_SRC_DIR)/build/share/gsoap/custom \
		-I$(GSOAP_SRC_DIR)/build/share/gsoap/plugin \
		-Isrc/generated \
		$(GSOAP_SRC_DIR)/gsoap/dom.c \
		$(GSOAP_SRC_DIR)/gsoap/stdsoap2.c \
		$(GSOAP_SRC_DIR)/build/share/gsoap/plugin/wsaapi.c \
		$(GSOAP_SRC_DIR)/build/share/gsoap/plugin/wsddapi.c \
		src/generated/soapClient.c \
		src/generated/soapC.c \
		src/onvif_discovery.c \
        src/discoverer.c \
			`pkg-config --libs openssl` \
			`pkg-config --libs zlib` \
			`pkg-config --libs gtk+-3.0` \
            `pkg-config --libs glib-2.0` \
			`PKG_CONFIG_PATH=$(GSOAP_SRC_DIR)/build/lib/pkgconfig pkg-config --libs gsoap` \
			`PKG_CONFIG_PATH=$(GSOAP_SRC_DIR)/build/lib/pkgconfig pkg-config --libs gsoapssl`
	@gcc -shared -o build/lib/libonvif_discovery.so wsddapi.o wsaapi.o stdsoap2.o soapClient.o soapC.o dom.o onvif_discovery.o discoverer.o
	@mkdir -p build/include
	@cp src/*.h build/include
	@rm -rf *.o

