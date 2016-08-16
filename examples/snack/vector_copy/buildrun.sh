#!/bin/bash

# Compile accelerated functions
echo 
if [ -f vector_copy.o ] ; then rm vector_copy.o ; fi
snack.sh -q -c vector_copy.cl 

# Compile Main and link to accelerated functions in vector_copy.o
if [ -f VectorCopy ] ; then rm VectorCopy ; fi
echo g++ -o VectorCopy vector_copy.o VectorCopy.cpp -L/opt/rocm/lib -lhsa-runtime64 
g++ -o VectorCopy vector_copy.o VectorCopy.cpp -L/opt/rocm/lib -lhsa-runtime64 
#  Execute
echo ./VectorCopy
./VectorCopy
