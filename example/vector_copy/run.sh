#LD_LIBRARY_PATH=:/home/user1/work/workspace/hsa/runtime/core/build/lnx64a/so/B_dbg/
OBSEDIAN_RUNTIME=/home/user1/work/dev_work/foundation/obsedian/release.hsa_foundation/lib/x86_64
THUNK=/home/user1/work/drivers/0623/thunk/lnx64a
LD_LIBRARY_PATH=$OBSEDIAN_RUNTIME
LD_LIBRARY_PATH=$THUNK:$LD_LIBRARY_PATH
export  LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH
./vector_copy
