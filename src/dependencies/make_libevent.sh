echo '-----begin make libvent------------'

cd libevent-2.1.8-stable_linux
./autogen.sh
./configure
make
cp .libs/libevent.a ../../../bin/Debug/
cp .libs/libevent.a ../../../bin/Release/

cp .libs/libevent.so ../../../bin/Debug/
cp .libs/libevent.so ../../../bin/Release/

echo '-----finish make libvent------------'