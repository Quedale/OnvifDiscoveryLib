AUTOMAKE_OPTIONS = foreign


discoverylib$(EXEEXT):
	@echo "Compiling Discovery Library..."
	@mkdir -p build
	@mkdir -p build/lib
	@gcc -c -Wall -fpic -O2 -flto \
		`PKG_CONFIG_PATH=$(shell pwd)/gsoap-2.8/build/lib/pkgconfig pkg-config --cflags gsoap` \
		`PKG_CONFIG_PATH=$(shell pwd)/gsoap-2.8/build/lib/pkgconfig pkg-config --cflags gsoapssl` \
		`pkg-config --cflags openssl` \
		`pkg-config --cflags zlib` \
		-Igsoap-2.8/ \
		-Igsoap-2.8/build/include/ \
		-Igsoap-2.8/build/share/gsoap/import \
		-Igsoap-2.8/build/share/gsoap/custom \
		-Igsoap-2.8/build/share/gsoap/plugin \
		-Isrc/generated \
		gsoap-2.8/gsoap/dom.c \
		gsoap-2.8/gsoap/stdsoap2.c \
		gsoap-2.8/build/share/gsoap/plugin/wsaapi.c \
		gsoap-2.8/build/share/gsoap/plugin/wsddapi.c \
		src/generated/soapClient.c \
		src/generated/soapC.c \
		src/onvif_discovery.c \
			`pkg-config --libs openssl` \
			`pkg-config --libs zlib` \
			`PKG_CONFIG_PATH=$(shell pwd)/gsoap-2.8/build/lib/pkgconfig pkg-config --libs gsoap` \
			`PKG_CONFIG_PATH=$(shell pwd)/gsoap-2.8/build/lib/pkgconfig pkg-config --libs gsoapssl`
	@gcc -shared -o build/lib/libonvif_discovery.so wsddapi.o wsaapi.o stdsoap2.o soapClient.o soapC.o dom.o onvif_discovery.o