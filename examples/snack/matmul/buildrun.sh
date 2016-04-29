#!/bin/bash

#   For this test case we need libbsd-dev for the random number generator
#   sudo apt-get install libbsd-dev

export LD_LIBRARY_PATH=/opt/rocm/hsa/lib

snkcmd=snack.sh # could also use snackhsail.sh

# Compile accelerated functions
echo 
if [ -f matmulKernels.o ] ; then rm matmulKernels.o ; fi
echo "$snkcmd -v -c  matmulKernels.cl "
$snkcmd -v -c  matmulKernels.cl 

# Compile Main .c  and link to accelerated functions in matmulKernels.o
echo 
if [ -f matmul ] ; then rm matmul ; fi
echo "gcc -O3 -o matmul matmulKernels.o matmul.c -L/opt/rocm/hsa/lib -lhsa-runtime64 -lbsd"
gcc -O3 -o matmul matmulKernels.o matmul.c -L/opt/rocm/hsa/lib -lhsa-runtime64 -lbsd

#  Execute the application
echo 
#  Make sure parci
#./matmul 5 6 7
#./matmul 2000 2000 2000
./matmul 2048 2048 2048
