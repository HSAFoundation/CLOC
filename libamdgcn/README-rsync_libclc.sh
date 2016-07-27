
#
#  This script copies the libclc github repository into the 
#  libamdgcn directory and excludes items not used.
#

cd $HOME/git/cloc/libamdngcn
rsync -avC --exclude ".git" --exclude "amdgcn--/" \
           --exclude "generic--/" --exclude "nvptx--nvidiacl"  \
           --exclude "r600--/" --exclude "nvptx64--nvidiacl"  \
           --exclude "ptx/" --exclude "ptx-nvidiacl"  \
           --exclude "test/" --exclude "www/"  \
           --exclude "amdgcn-amdhsa/" --exclude "compile-test.sh"  \
           --exclude "configure.py" --exclude "Makefile"  \
           --exclude "libclc.pc" --exclude ".gitignore"  \
           --exclude "r600/" --exclude "prepare-builtins.o.d"  \
           --exclude "build/" $HOME/git/libclc .

