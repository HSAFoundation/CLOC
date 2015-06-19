#!/bin/bash

#  Set HSA Environment variables
[ -z $HSA_RUNTIME_PATH ] && HSA_RUNTIME_PATH=/opt/hsa
[ -z $HSA_LIBHSAIL_PATH ] && HSA_LIBHSAIL_PATH=/opt/hsa/lib
[ -z $HSA_LLVM_PATH ] && HSA_LLVM_PATH=/opt/amd/cloc/bin
[ -z $SNACK_RUNTIME_PATH ] && SNACK_RUNTIME_PATH=$HOME/git/CLOC.ashwinma
SNACK_INC=$SNACK_RUNTIME_PATH/include
export LD_LIBRARY_PATH=$HSA_RUNTIME_PATH/lib:$SNACK_RUNTIME_PATH/lib:$LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH
# Compile accelerated functions
echo 
if [ -f hw.o ] ; then rm hw.o ; fi
echo snack.sh -snk $SNACK_RUNTIME_PATH -v -gccopt 3 hw.cl
snack.sh -snk $SNACK_RUNTIME_PATH -v -gccopt 3 hw.cl
