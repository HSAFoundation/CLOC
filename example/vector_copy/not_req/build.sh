#Pre req - Install development version of libelf
#sudo apt-get install libelf-dev


RUNTIME_PATH=$DRIVERS/hsa/runtime/
COMPILER_PATH=$DRIVERS/hsa/compiler/finalizer/Interface
ASSEMBLER_PATH=$DRIVERS/hsa/compiler/hsail-tools/libHSAIL
BUILD_DIR=build/lnx64a/B_dbg/

g++ -g -c -I $RUNTIME_PATH/inc -I $COMPILER_PATH -o elf_utils.o elf_utils.cpp -std=c++0x
g++ -g -c -I $RUNTIME_PATH/inc -I $COMPILER_PATH -I $ASSEMBLER_PATH -o vector_copy.o obsedian_vector_copy.cpp -std=c++0x
g++ -g -c -I $RUNTIME_PATH/inc -I $COMPILER_PATH -I $ASSEMBLER_PATH -I $ASSEMBLER_PATH/$BUILD_DIR -o assemble.o assemble.cpp -std=c++0x

g++ -g  -Wl,--unresolved-symbols=ignore-in-shared-libs -o vector_copy assemble.o elf_utils.o vector_copy.o $ASSEMBLER_PATH/$BUILD_DIR/LIBHSAIL.a -lelf -L$RUNTIME_PATH/core/build/lnx64a/so/B_dbg/ -lhsa-runtime64  
