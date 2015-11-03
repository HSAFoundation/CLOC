#!/bin/bash

#  Set HSA Environment variables
[ -z $HSA_RUNTIME_PATH ] && HSA_RUNTIME_PATH=/opt/hsa

export LD_LIBRARY_PATH=$HSA_RUNTIME_PATH/lib

# Compile accelerated functions
echo 
if [ -f CSquares.o ] ; then rm CSquares.o ; fi
echo snack.sh -c CSquares.cl 
snack.sh -c CSquares.cl 

# Compile Main and link to accelerated functions in CSquares.o
echo 
if [ -f CSquares ] ; then rm CSquares ; fi
echo "g++ -o CSquares CSquares.o CSquares.cpp -L$HSA_RUNTIME_PATH/lib -lhsa-runtime64  "
g++ -o CSquares CSquares.o CSquares.cpp -L$HSA_RUNTIME_PATH/lib -lhsa-runtime64 

#  Execute
echo
echo ./CSquares
./CSquares
