make clean
make
echo  "\n\n make done"
#fuser -k 9000/tcp
./src/gtk 127.0.0.1 9000
echo  "\n\n run done"
