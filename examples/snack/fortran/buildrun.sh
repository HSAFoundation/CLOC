#!/bin/bash

export LD_LIBRARY_PATH=/opt/rocm/hsa/lib

#  First compile the acclerated functions to create hw.o
#  Tell cloc to use fortran names for external references
echo snack.sh -fort -c hw.cl 
snack.sh -fort -c hw.cl 

#  Compile the main Fortran program and link to hw.o
echo f95 -fcray-pointer -o HelloWorld hw.o HelloWorld.f -L/opt/rocm/hsa/lib -lhsa-runtime64 
mymcpu=`mymcpu`
mymcpu=${mymcpu:-fiji}
if [ "$mymcpu" == "fiji" ] ; then 
   malloc_trigger="-DGLOBALMALLOC"
else
   malloc_trigger=""
fi
echo f95 -cpp -fcray-pointer $malloc_trigger -o HelloWorld hw.o HelloWorld.f -L/opt/rocm/hsa/lib -lhsa-runtime64 
f95 -cpp -fcray-pointer $malloc_trigger -o HelloWorld hw.o HelloWorld.f -L/opt/rocm/hsa/lib -lhsa-runtime64 

echo ./HelloWorld
./HelloWorld
