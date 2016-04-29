#!/bin/bash

#  This is a demo to show that cloc can work with multiple cl files. 

export LD_LIBRARY_PATH=/opt/rocm/hsa/lib

#  First compile all files with acclerated functions to create hw.o and hw2.o
echo 
echo snack.sh -c hw.cl 
snack.sh -c hw.cl 
echo 
echo snack.sh -c -noglobs hw2.cl 
snack.sh -c -noglobs hw2.cl 

#  Compile the main program and link to hw.o
#  Main program can be c, cpp, or fotran
echo 
echo "gcc -o HelloWorld hw.o hw2.o HelloWorld.c -L/opt/rocm/hsa/lib -lhsa-runtime64 "
gcc -o HelloWorld hw.o hw2.o HelloWorld.c -L/opt/rocm/hsa/lib -lhsa-runtime64 

echo 
echo ./HelloWorld
./HelloWorld
