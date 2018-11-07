echo '-----begin make protobuf------------'

cd protobuf-all-3.6.1
chmod 777 configure
./configure
make
cp src/.libs/libprotobuf.a ../../../bin/Debug/
cp src/.libs/libprotobuf.a ../../../bin/Release/

cp src/.libs/libprotobuf.so ../../../bin/Debug/
cp src/.libs/libprotobuf.so ../../../bin/Release/

echo '-----finish make protobuf------------'