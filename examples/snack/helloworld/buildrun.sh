#!/bin/bash

#  Call buildrun.sh as follows

#  ./buildrun 
#  ./buildrun cpp
#  ./buildrun f
#
function getdname(){
   local __DIRN=`dirname "$1"`
   if [ "$__DIRN" = "." ] ; then 
      __DIRN=$PWD; 
   else
      if [ ${__DIRN:0:1} != "/" ] ; then 
         if [ ${__DIRN:0:2} == ".." ] ; then 
               __DIRN=`dirname $PWD`/${__DIRN:3}
         else
            if [ ${__DIRN:0:1} = "." ] ; then 
               __DIRN=$PWD/${__DIRN:2}
            else
               __DIRN=$PWD/$__DIRN
            fi
         fi
      fi
   fi
   echo $__DIRN
}

example_dir=$(getdname $0)

#  Compile the acclerated functions with snack.sh to create hw.o
echo 
echo "cd $example_dir"
cd $example_dir

export LD_LIBRARY_PATH=/opt/hsa/lib

#  Compile the acclerated functions with snack.sh to create hw.o
echo 
if [ "$1" == "f" ] ; then 
   echo "snack.sh -fort -c hw.cl"
   snack.sh -fort -c hw.cl 
else
   echo "snack.sh -c hw.cl "
   snack.sh -c hw.cl 
fi

#  Compile the main program and link to hw.o
#  Main program can be c, cpp, or fotran
echo 
if [ "$1" == "cpp" ] ; then 
   echo "g++ -o HelloWorld hw.o HelloWorld.cpp -L/opt/hsa/lib -lhsa-runtime64"
   g++ -o HelloWorld hw.o HelloWorld.cpp -L/opt/hsa/lib -lhsa-runtime64  
elif [ "$1" == "f" ] ; then 
   echo "f95 -o HelloWorld hw.o HelloWorld.f -L/opt/hsa/lib -lhsa-runtime64"
   f95 -o HelloWorld hw.o HelloWorld.f -L/opt/hsa/lib -lhsa-runtime64  
else
   echo "gcc -o HelloWorld hw.o HelloWorld.c -L/opt/hsa/lib -lhsa-runtime64"
   gcc -o HelloWorld hw.o HelloWorld.c -L/opt/hsa/lib -lhsa-runtime64 
fi

# Run the program
echo 
echo ./HelloWorld
./HelloWorld

