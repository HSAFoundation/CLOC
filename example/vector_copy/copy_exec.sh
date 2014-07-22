SERVER=$1
SHREYAS_PATH=~/Perforce/sramalin_runtime/drivers/opencl/dist
BKENDALL_PATH=~/work/dist/bkendall_server/dist


DIST=$SHREYAS_PATH 
if [ $SERVER ] ;   then
        if [ $SERVER == bkendall ] ; then
            DIST=$BKENDALL_PATH
            #copy runtime and libhsail - temp
            cp -a $BKENDALL_PATH/../libHSAIL .
            cp -a $BKENDALL_PATH/../hsa .
        fi    
    fi

cp $DIST/linux/debug.hsa/bin/x86_64/clc* dist/
cp $DIST/linux/debug.hsa/bin/x86_64/ll* dist/
cp $DIST/linux/debug.hsa/bin/x86_64/*.bc dist/
cp $DIST/linux/debug.hsa/lib/x86_64/*.bc dist/
cp $DIST/linux/debug.hsa/bin/x86_64/opt* dist/
cp $DIST/linux/debug.hsa/bin/x86_64/HSAILasm dist/
cp $DIST/linux/debug.hsa/bin/x86_64/hsailasm dist/HSAILasm


