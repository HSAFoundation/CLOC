#!/bin/bash

#  Set HSA Environment 
[ -z $HSA_RUNTIME_PATH ] && HSA_RUNTIME_PATH=/opt/hsa
export LD_LIBRARY_PATH=$HSA_RUNTIME_PATH/lib

# Compile accelerated functions
echo 
if [ -f sumKernel.o ] ; then rm sumKernel.o ; fi
echo snack.sh -c sumKernel.cl 
snack.sh -c sumKernel.cl 

echo 
if [ -f vecsum ] ; then rm vecsum ; fi
echo g++ -O3  -o vecsum sumKernel.o vecsum.cpp -L $HSA_RUNTIME_PATH/lib -lhsa-runtime64  
g++ -O3  -o vecsum sumKernel.o vecsum.cpp -L $HSA_RUNTIME_PATH/lib -lhsa-runtime64 

#  Execute
echo
echo ./vecsum 
./vecsum | tee vecsum.out
