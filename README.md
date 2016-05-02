CLOC - V 1.0.10 
===============

CLOC:  CL Offline Compiler
       Generate HSA code object from a cl (Kernel c Language) file.
SNACK: Structured No API Compiled Kernels.
       Launch GPU kernels as host-callable functions with structured launch parameters.

Table of contents
-----------------

- [Copyright and Disclaimer](#Copyright)
- [Software License Agreement](LICENSE.TXT)
- [Command Help](#CommandHelp)
- [Examples](#ReadmeExamples)
- [Install](INSTALL.md)

<A NAME="Copyright">
# Copyright and Disclaimer
------------------------

Copyright (c) 2016 ADVANCED MICRO DEVICES, INC.  

AMD is granting you permission to use this software and documentation (if any) (collectively, the 
Materials) pursuant to the terms and conditions of the Software License Agreement included with the 
Materials.  If you do not have a copy of the Software License Agreement, contact your AMD 
representative for a copy.

You agree that you will not reverse engineer or decompile the Materials, in whole or in part, except for 
example code which is provided in source code form and as allowed by applicable law.

WARRANTY DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
KIND.  AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT 
LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE SOFTWARE WILL RUN UNINTERRUPTED OR ERROR-
FREE OR WARRANTIES ARISING FROM CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK 
ASSOCIATED WITH THE USE OF THE SOFTWARE IS ASSUMED BY YOU.  Some jurisdictions do not 
allow the exclusion of implied warranties, so the above exclusion may not apply to You. 

LIMITATION OF LIABILITY AND INDEMNIFICATION:  AMD AND ITS LICENSORS WILL NOT, 
UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT, INCIDENTAL, 
INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF THE SOFTWARE OR THIS 
AGREEMENT EVEN IF AMD AND ITS LICENSORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH 
DAMAGES.  In no event shall AMD's total liability to You for all damages, losses, and 
causes of action (whether in contract, tort (including negligence) or otherwise) 
exceed the amount of $100 USD.  You agree to defend, indemnify and hold harmless 
AMD and its licensors, and any of their directors, officers, employees, affiliates or 
agents from and against any and all loss, damage, liability and other expenses 
(including reasonable attorneys' fees), resulting from Your use of the Software or 
violation of the terms and conditions of this Agreement.  

U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with "RESTRICTED RIGHTS." 
Use, duplication, or disclosure by the Government is subject to the restrictions as set 
forth in FAR 52.227-14 and DFAR252.227-7013, et seq., or its successor.  Use of the 
Materials by the Government constitutes acknowledgement of AMD's proprietary rights in them.

EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as stated in the 
Software License Agreement.

<A NAME="CommandHelp">
# Command Help 
--------- 
## The cloc.sh command 

```
   cloc.sh: Compile a cl file into an HSA Code object file (.hsaco)  
            using the LLVM Ligntning Compiler. An hsaco file contains 
            the amdgpu isa that can be loaded by the HSA Runtime.

   Usage: cloc.sh [ options ] filename.cl

   Options without values:
    -ll       Generate dissassembled LLVM IR, for info only
    -g        Generate debug information
    -noqp     No quickpath, Use LLVM IR commands
    -version  Display version of cloc then exit
    -v        Verbose messages
    -n        Dryrun, do nothing, show commands that would execute
    -h        Print this help message
    -k        Keep temporary files
    -brig     Generate brig  (soon to be depracated)
    -hsail    Generate dissassembled hsail (soon to be deprecated)

   Options with values:
    -path    <path>           $CLOC_PATH or <cdir> if CLOC_PATH not set
                              <cdir> is directory where cloc.sh is found
    -amdllvm <path>           $AMDLLVM or /opt/amd/llvm
    -libgcn  <path>           $LIBGCN or /opt/rocm/libamdgcn  
    -hlcpath <path>           $HLC_PATH or /opt/rocm/hlc3.2/bin  
    -mcpu    <cputype>        Default= value returned by ./mymcpu
    -clopts  <compiler opts>  Default=" "
    -I       <include dir>    Provide one directory per -I option
    -lkopts  <LLVM link opts> Default=$LIBGCN/lib/libamdgcn.$mcpu.bc
    -hsaillib <fname>         Filename of hsail library.(soon to be deprecated)
    -opt     <LLVM opt>       LLVM optimization level
    -o       <outfilename>    Default=<filename>.<ft> ft=brig or hsail
    -t       <tdir>           Default=/tmp/cloc-tmp-$$, Temp dir for files

   Examples:
    cloc.sh my.cl             /* create my.hsaco                    */

   You may set these environment variables 
   LLVMOPT, CLOC_PATH,HLC_PATH,AMDLLVM,LIBGCN,LC_MCPU, CLOPTS, or LKOPTS 
   instead of providing these respective command line options 
   -opt, -path, -hlcpath, -amdllvm, -libgcn, -mcpu,  -clopts, or -lkopts 
   Command line options will take precedence over environment variables. 

   Copyright (c) 2016 ADVANCED MICRO DEVICES, INC.
```


## The snack.sh Command 

```
   snack.sh: Generate host-callable "snack" functions for GPU kernels.
             Snack generates the source code and headers for each kernel 
             in the input filename.cl file.  The -c option will compile 
             the source with gcc so you can link with your host application.
             Host applicaton requires no API to use snack functions.

   Usage: snack.sh [ options ] filename.cl

   Options without values:
    -c        Compile generated source code to create .o file
    -ll       Tell cloc.sh to generate disassembled LLVM IR 
    -version  Display version of snack then exit
    -v        Verbose messages
    -vv       Get additional verbose messages from cloc.sh
    -n        Dryrun, do nothing, show commands that would execute
    -h        Print this help message
    -k        Keep temporary files
    -fort     Generate fortran function names
    -noglobs  Do not generate global functions 
    -kstats   Print out code object kernel statistics 
    -m32      Generate snackwrap in 32-bit mode. If -c, also compile in 32
              bit mode

   Options with values:
    -path     <path>         $CLOC_PATH or <sdir> if CLOC_PATH not set
                             <sdir> is directory where snack.sh is found
    -mcpu     <cpu>          Default=`'mymcpu`, Options: kaveri,carrizo,fiji
    -amdllvm  <path>         Default=/opt/amd/llvm or env var AMDLLVM 
    -libgcn   <path>         Default=/opt/rocm/libamdgcn or env var LIBGCN 
    -opt      <LLVM opt>     Default=2, passed to cloc.sh to build code object
    -gccopt   <gcc opt>      Default=2, gcc optimization for snack wrapper
    -t        <tempdir>      Default=/tmp/snk_$$, Temp dir for files
    -s        <symbolname>   Default=filename 
    -hsart    <HSA RT>       Default=CLOC_PATH/..
    -o        <outfilename>  Default=<filename>.<ft> 
    -foption  <fnlizer opts> Default=""  Finalizer options

   Examples:
    snack.sh my.cl              /* create my.snackwrap.c and my.h    */
    snack.sh -c my.cl           /* gcc compile to create  my.o       */
    snack.sh -t /tmp/foo my.cl  /* will automatically set -k         */

   You may set environment variables CLOC_PATH, HSA_RT, AMDLLVM, LIBGCN
   instead of providing options -path, -hsart, -amdllvm, -libgcn respectively
   Command line options will take precedence over environment variables. 

   Copyright (c) 2016 ADVANCED MICRO DEVICES, INC.

```

<A NAME="ReadmeExamples">
# Examples
-------- 

## Example 1: Hello World

This version of cloc supports the SNACK method of writing accelerated 
kernels in c. With SNACK, the host program can directly call the accelerated 
function without an API such as OpenCL or CUDA.

Here is the c++ source code HelloWorld.cpp using SNACK.
```cpp
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#include "hw.h"
int main(int argc, char* argv[]) {
	const char* input_const = "Gdkkn\x1FGR@\x1FVnqkc";
	size_t strlength = strlen(input_const);
	char *output = (char*) malloc_global(strlength + 1);
	char *input = (char*) malloc_global(strlength + 1);
        strncpy(input,input_const,strlength);
        SNK_INIT_LPARM(lparm,strlength);
        decode(input,output,lparm);
	output[strlength] = '\0';
	cout << output << endl;
	free_global(output);
	free_global(input);
	return 0;
}
```
The c source for the accelerated kernel is in file hw.cl.
```c
/*
    File:  hw.cl 
*/
__kernel void decode(__global const char* in, __global char* out) {
	int num = get_global_id(0);
	out[num] = in[num] + 1;
}
```
The -c option of snack.sh will call the gcc compiler and create 
the object file and the header file "hw.h" required to build 
the host program. Without -c you get the generated c code hw.snackwrap.c 
and the header file.  The header file has the function prototypes 
for all kernels declared in the .cl file.  Use this command to 
compile the hw.cl file with snack.sh.

```
/opt/amd/cloc/bin/snack.sh -c hw.cl
```

You can now compile and build the binary "HelloWorld" with any c++ compiler.
Here is the command to build HelloWorld with g++. 

```
g++ -o HelloWorld hw.o HelloWorld.cpp -L/opt/hsa/lib -lhsa-runtime64 -lelf 

```

Then execute the program as follows.
```
export LD_LIBRARY_PATH=/opt/hsa/lib
$ ./HelloWorld
Hello HSA World
```

This example and other examples can be found in the cloc examples directory found in /opt/rocm/cloc/examples


## Example 2: Use of -hsaillib by cloc.sh and snackhsail.sh

The snack.sh command creates and embeds HSA code object or isa.  To create and embed the HSAIL intermediate
language you can use the snackhsail.sh command in an similar way. 

The snackhsail.sh command has an additional option to provide an HSAIL library.
This options specifies a single file with the hsail for multiple functions that can be 
called directly by cl.  One should have a corresponding header file for these functions.  
An example to use the -hsaillib option is provided in the directory examples/snack/test_hsail_lib. 
This example uses the simple HSAIL library provided in the directory examples/mathdemo_hsaillib

Here you see the header file and the corresponding hsail file. This is the contents of 
the header file mathdemo_hsaillib.h. 
```
float __sin(float in);
float __cos(float in);
float __exp(float in);
```
The example cl file (examples/snack/test_hsail_lib/test_hsail_lib.cl) includes the above header file. 
```
#include "../../mathdemo_hsaillib/mathdemo_hsaillib.h"
__kernel void testkernel( __global float * outfval , __global const float  * fval) {
   int i = get_global_id(0);
   outfval[i] =  __sin(fval[i]);
}
```
If you want to compile and build the example, execute the buildrun.sh script found
in the example directory.  It calls snackhsail.sh as follows:
```
snackhsail.sh  -v -c  -hsaillib ../../mathdemo_hsaillib/mathdemo_hsaillib.hsail test_hsail_lib.cl
```
If you are just building the hsail for the above kernel, provide the -hsaillib option to cloc.sh
as follows.
```
cloc.sh -hsail -hsaillib ../../mathdemo_hsaillib/mathdemo_hsaillib.hsail test_hsail_lib.cl
```
Your resulting hsail test_hsail_lib.hsail will include the hsaillib inserted into the correct
position. The generated brig will also include the library. 

## Example 3: Creating code object file with GCN isa and loading it with HSA API.

In it's simplest form, cloc.sh compiles a .cl file into an HSA code object file.   This is a standard ELF file that can be loaded by the HSA API without the need for finalization.   However, the rest of the HSA API is required to launch this object code to the GPU.  An example is provided in the examples/hsa directory that shows the necessary HSA API to launch an HSA code object.  Use these commands to run this example:
```
cd $HOME
cp -rp /opt/rocm/cloc/examples/hsa/vector_copy_codeobject /tmp
cd /tmp/vector_copy_codeobject
make
make test
```
The make command will compile vector_copy_codeobject.cpp with these commands:
```
g++ -c -std=c++11 -I/opt/rocm/hsa/include -o obj/vector_copy_codeobject.o vector_copy_codeobject.cpp
g++ obj/vector_copy_codeobject.o -L/opt/rocm/hsa/lib -lhsa-runtime64 -o vector_copy_codeobject
```
It then calls cloc.sh to create the file vectory_copy_codeobject.hsaco with this command. 
```
cloc.sh vector_copy_codeobject.cl
```
The program will load the HSA code object file and launch it with the HSA API.  One could compare the minor difference in the HSA API between loading a code object and loading brig by comparing the source code and Makefile from the above example to the vector_copy example found in /opt/rocm/cloc/examples/hsa/vector_copy.   In the vector_copy example, cloc.sh requires the -brig option and the API source code in vector_copy.c has the extra finalization step. 
