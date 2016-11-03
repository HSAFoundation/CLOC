#!/bin/bash

#  Call buildrun.sh as follows

#  ./buildrun 
#  ./buildrun cpp
#  ./buildrun f
#
#  Compile the acclerated functions with snack.sh to create hw.o
if [ "$1" == "f" ] ; then 
   echo snack.sh -fort -c hw.cl 
   snack.sh -fort -c hw.cl 
else
   echo snack.sh -c hw.cl 
   snack.sh -c hw.cl 
fi

#  Compile the main program and link to hw.o
#  Main program can be c, cpp, or fotran
if [ "$1" == "cpp" ] ; then 
   echo g++ -o HelloWorld hw.o HelloWorld.cpp -L/opt/rocm/lib -lhsa-runtime64  
   g++ -o HelloWorld hw.o HelloWorld.cpp -L/opt/rocm/lib -lhsa-runtime64  
elif [ "$1" == "f" ] ; then 
   if [ -f /usr/bin/mymcpu ] ; then 
      mymcpu=`/usr/bin/mymcpu`
   else 
      mymcpu=${mymcpu:-fiji}
   fi
   if [ "$mymcpu" == "fiji" ] ; then 
      malloc_trigger="-DGLOBALMALLOC"
   else
      malloc_trigger=""
   fi
   echo f95 -cpp -fcray-pointer $malloc_trigger -o HelloWorld hw.o HelloWorld.f -L/opt/rocm/lib -lhsa-runtime64 
   f95 -cpp -fcray-pointer $malloc_trigger -o HelloWorld hw.o HelloWorld.f -L/opt/rocm/lib -lhsa-runtime64 
else
   echo gcc -o HelloWorld hw.o HelloWorld.c -L/opt/rocm/lib -lhsa-runtime64 
   gcc -o HelloWorld hw.o HelloWorld.c -L/opt/rocm/lib -lhsa-runtime64 
fi

# Run the program
echo ./HelloWorld
./HelloWorld

