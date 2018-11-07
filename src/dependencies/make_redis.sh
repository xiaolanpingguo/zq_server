echo '-----begin make hiredis------------'

cd hiredis/hiredis_linux/hiredis
make 
cp libhiredis.a ../../../../../bin/Debug/
cp libhiredis.a ../../../../../bin/Release/

cp libhiredis.so ../../../../../bin/Debug/
cp libhiredis.so ../../../../../bin/Release/

make clean

echo '-----finish make hiredis------------'
