#!/bin/bash

export LD_LIBRARY_PATH=/opt/hsa/lib

#  Compile the acclerated functions with snack.sh to create hw.o
echo 
if [ "$1" == "f" ] ; then 
   echo "snack.sh -fort -c hw.cl"
   snack.sh -fort -c hw.cl 
else
   echo "snack.sh -c hw.cl "
   snack.sh -c hw.cl 
fi

#  Compile the main program and link to hw.o
#  Main program can be c, cpp, or fotran
echo 
if [ "$1" == "cpp" ] ; then 
   echo "g++ -o HelloWorld hw.o HelloWorld.cpp -L/opt/hsa/lib -lhsa-runtime64"
   g++ -o HelloWorld hw.o HelloWorld.cpp -L/opt/hsa/lib -lhsa-runtime64  
elif [ "$1" == "f" ] ; then 
   echo "f95 -o HelloWorld hw.o HelloWorld.f -L/opt/hsa/lib -lhsa-runtime64"
   f95 -o HelloWorld hw.o HelloWorld.f -L/opt/hsa/lib -lhsa-runtime64  
else
   echo "gcc -o HelloWorld hw.o HelloWorld.c -L/opt/hsa/lib -lhsa-runtime64"
   gcc -o HelloWorld hw.o HelloWorld.c -L/opt/hsa/lib -lhsa-runtime64 
fi

# Run the program
echo 
echo ./HelloWorld
./HelloWorld

