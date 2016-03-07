#!/bin/bash

#  Set HSA Environment variables
[ -z $HSA_RUNTIME_PATH ] && HSA_RUNTIME_PATH=/opt/hsa

export LD_LIBRARY_PATH=$HSA_RUNTIME_PATH/lib

# Compile accelerated functions
echo 
if [ -f test_hsail_lib.o ] ; then rm test_hsail_lib.o ; fi
echo "/opt/amd/cloc/bin/snackhsail.sh -v -c -hsaillib ../../mathdemo_hsaillib/mathdemo_hsaillib.hsail test_hsail_lib.cl "
/opt/amd/cloc/bin/snackhsail.sh -v -c -hsaillib ../../mathdemo_hsaillib/mathdemo_hsaillib.hsail test_hsail_lib.cl 

# Compile Main and link to accelerated functions in test_hsail_lib.o
echo 
if [ -f test_hsail_lib ] ; then rm test_hsail_lib ; fi

echo g++ -o test_hsail_lib test_hsail_lib.o test_hsail_lib.cpp -L$HSA_RUNTIME_PATH/lib -lhsa-runtime64 -lm
g++ -o test_hsail_lib test_hsail_lib.o test_hsail_lib.cpp -L$HSA_RUNTIME_PATH/lib -lhsa-runtime64 -lm

#  Execute
echo
echo ./test_hsail_lib
./test_hsail_lib
