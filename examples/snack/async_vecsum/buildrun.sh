#!/bin/bash

# Compile accelerated functions
echo 
if [ -f sumKernel.o ] ; then rm sumKernel.o ; fi
echo snack.sh -c sumKernel.cl 
snack.sh -c sumKernel.cl 

echo 
if [ -f vecsum ] ; then rm vecsum ; fi
echo g++ -O3  -o vecsum sumKernel.o vecsum.cpp -L /opt/rocm/lib -lhsa-runtime64  
g++ -O3  -o vecsum sumKernel.o vecsum.cpp -L /opt/rocm/lib -lhsa-runtime64 

#  Execute
echo
echo ./vecsum 
./vecsum | tee vecsum.out
