#!/bin/bash

export LD_LIBRARY_PATH=/opt/hsa/lib

#  First compile the acclerated functions to create hw.o
#  Tell cloc to use fortran names for external references
echo snack.sh -fort -c hw.cl 
snack.sh -fort -c hw.cl 

#  Compile the main Fortran program and link to hw.o
echo f95 -fcray-pointer -o HelloWorld hw.o HelloWorld.f -L/opt/hsa/lib -lhsa-runtime64 
f95 -fcray-pointer -o HelloWorld hw.o HelloWorld.f -L/opt/hsa/lib -lhsa-runtime64 

echo ./HelloWorld
./HelloWorld
