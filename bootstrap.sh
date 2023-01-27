SKIP_GSOAP=0
GSOAP_SRC_DIR="${GSOAP_SRC_DIR:=gsoap-2.8}" 
i=1;
for arg in "$@" 
do
    if [ $arg == "--skip-gsoap" ]; then
        SKIP_GSOAP=1
    fi
    i=$((i + 1));
done

sudo apt install automake autoconf gcc make pkg-config
sudo apt install unzip

if [ $SKIP_GSOAP -eq 0 ]; then
    echo "-- Building gsoap libgsoap-dev --"
    #WS-Security depends on OpenSSL library 3.0 or 1.1
    wget https://sourceforge.net/projects/gsoap2/files/gsoap_2.8.123.zip/download
    unzip download
    rm download
    cd gsoap-2.8
    mkdir build
    ./configure --with-openssl=/usr/lib/ssl --prefix=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/build
    LIBRARY_PATH="$(pkg-config --variable=libdir openssl):$LIBRARY_PATH" \
    LD_LIBRARY_PATH="$(pkg-config --variable=libdir openssl):$LD_LIBRARY_PATH" \
        make -j$(nproc)
    make install
    cd ..
fi

aclocal
autoconf
automake --add-missing
autoreconf -i

rm -rf $(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/src/generated
mkdir $(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/src/
mkdir $(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/src/generated
wsdl2h -x -t $(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/wsdl/typemap.dat -o $(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/src/generated/discovery.h -c \
https://www.onvif.org/onvif/ver10/network/wsdl/remotediscovery.wsdl \
http://schemas.xmlsoap.org/ws/2005/04/discovery/ws-discovery.wsdl 
soapcpp2 -CL -2 -x -I$GSOAP_SRC_DIR/gsoap/import:$GSOAP_SRC_DIR/gsoap $(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/src/generated/discovery.h -d$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)/src/generated