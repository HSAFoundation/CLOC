#!/bin/bash

incfile=$1
MCPU=$2

echo " " >$incfile
echo "# $incfile:  Generated from SOURCES file " >>$incfile
echo "#            for MCPU = $MCPU " >>$incfile
echo " " >>$incfile

for pair in `cat SOURCES` ; do 
  i="${pair##*:}"
  source_type="${pair%:*}"
  typefile="${i##*.}"
  filename="${i##*/}"
  dirname="${i%/*}"
  echo "\$(BCDIR)/${i}.$MCPU.bc: \$($source_type)/${i}" >>$incfile
  if [ $typefile == "cl" ] ; then 
     echo "	@echo CL $source_type: $i  -mcpu=$MCPU" >>$incfile
     echo "	\$(Verb) mkdir -p \$(BCDIR)/$dirname" >>$incfile
     echo "	\$(Verb) mkdir -p \$(DEPSDIR)/$dirname" >>$incfile
     echo "	\$(Verb) \$(LLVM_CC) \$(CLFLAGS) -mcpu=$MCPU -I\$($source_type)/$dirname  -MMD -MF \$(DEPSDIR)/${i}.d -o \$(BCDIR)/$i.$MCPU.bc \$($source_type)/${i}" >> $incfile
  else
     echo "	@echo AS $source_type: $i  -mcpu=$MCPU" >>$incfile
     echo "	\$(Verb) mkdir -p \$(BCDIR)/$dirname " >>$incfile
     echo "	\$(Verb) \$(LLVM_AS) -o \$(BCDIR)/$i.$MCPU.bc \$($source_type)/${i}" >>$incfile
  fi
  echo " " >>$incfile
done

echo " " >>$incfile

echo "# The big list of needed bc files to link together " >>$incfile
objline="OBJ$MCPU ="
for pair in `cat SOURCES` ; do 
  i="${pair##*:}"
  objline="${objline} \$(BCDIR)/${i}.$MCPU.bc"
done
echo "${objline}" >>$incfile

echo " " >>$incfile

echo "\$(BCDIR)/convert.$MCPU.bc: \$(UTILDIR)/convert.cl " >>$incfile
echo "	@echo CL convert.cl - $MCPU" >>$incfile
echo "	\$(Verb) mkdir -p \$(BCDIR)" >>$incfile
echo "	\$(Verb) \$(LLVM_CC) \$(CLFLAGS) -mcpu=$MCPU -I\$(LIBAMDGCN)/libclc_overrids/include -I\$(LIBCLC)/generic/lib -MMD -MF \$(DEPSDIR)/convert.cl.d -o \$(BCDIR)/convert.$MCPU.bc \$(UTILDIR)/convert.cl " >>$incfile

echo " " >>$incfile

echo "\$(BCDIR)/subnormal_disable.$MCPU.bc: \$(LIBCLC)/generic/lib/subnormal_disable.ll" >>$incfile
echo "	@echo AS subnormal_disable.ll" >>$incfile
echo "	\$(Verb) \$(LLVM_AS) -o \$(BCDIR)/subnormal_disable.$MCPU.bc \$(LIBCLC)/generic/lib/subnormal_disable.ll" >>$incfile
echo " " >>$incfile
echo "\$(BCDIR)/builtins.link.$MCPU.bc: \$(OBJ$MCPU) \$(BCDIR)/convert.$MCPU.bc " >>$incfile
echo "	@echo LINK all bc files for $MCPU" >>$incfile
echo "	\$(Verb) \$(LLVM_LINK) --suppress-warnings \$(OBJ$MCPU) \$(BCDIR)/convert.$MCPU.bc -o \$(BCDIR)/builtins.link.$MCPU.bc" >>$incfile

echo " " >>$incfile

echo "\$(BCDIR)/builtins.opt.$MCPU.bc: \$(BCDIR)/builtins.link.$MCPU.bc " >>$incfile
echo "	@echo LLVM-OPT -O3" >>$incfile
echo "	\$(Verb) \$(LLVM_OPT) -O3 -o \$(BCDIR)/builtins.opt.$MCPU.bc \$(BCDIR)/builtins.link.$MCPU.bc" >>$incfile

echo " " >>$incfile

echo "\$(BCDIR)/libamdgcn.$MCPU.bc: \$(BCDIR)/builtins.opt.$MCPU.bc \$(UTILDIR)/prepare-builtins" >>$incfile
echo "	@echo LAST STEP! Call prepare-builtins to create libamdgcn.$MCPU.bc">>$incfile
echo "	\$(Verb) \$(UTILDIR)/prepare-builtins -o \$(BCDIR)/libamdgcn.$MCPU.bc \$(BCDIR)/builtins.opt.$MCPU.bc" >>$incfile

echo " " >>$incfile

echo "$MCPU: \$(UTILDIR)/makefile.$MCPU.inc \$(BCDIR)/libamdgcn.$MCPU.bc \$(BCDIR)/subnormal_disable.$MCPU.bc " >>$incfile
