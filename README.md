CLOC - V 0.9.7 (HSA 1.0F) 
=========================

CLOC:  CL Offline Compiler
       Generate HSAIL or brig from a cl (Kernel c Language) file.
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

Copyright (c) 2015 ADVANCED MICRO DEVICES, INC.  

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
   cloc.sh: Convert a cl file to brig or hsail using the
            LLVM to HSAIL backend compiler.

   Usage: cloc.sh [ options ] filename.cl

   Options without values:
    -hsail    Generate dissassembled hsail from brig 
    -ll       Generate dissassembled ll from bc, for info only
    -version  Display version of cloc then exit
    -v        Verbose messages
    -n        Dryrun, do nothing, show commands that would execute
    -h        Print this help message
    -k        Keep temporary files

   Options with values:
    -t       <tdir>           Default=/tmp/cloc$$, Temp dir for files
    -o       <outfilename>    Default=<filename>.<ft> ft=brig or hsail
    -opt     <LLVM opt>       Default=2, LLVM optimization level
    -p       <path>           $HSA_LLVM_PATH or <cdir> if HSA_LLVM_PATH not set
                              <cdir> is actual directory of cloc.sh 
    -clopts  <compiler opts>  Default="-cl-std=CL2.0"
    -I       <include dir>    Provide one directory per -I option
    -lkopts  <LLVM link opts> Default="-prelink-opt   \
              -l <cdir>/builtins-hsail.bc -l <cdir>/builtins-gcn.bc   \
              -l <cdir>/builtins-hsail-amd-ci.bc"
    -hsaillib <fname>         Filename of hsail library.

   Examples:
    cloc.sh my.cl               /* create my.brig                   */
    cloc.sh  -hsail my.cl       /* create my.hsail and my.brig      */

   You may set environment variables LLVMOPT, HSA_LLVM_PATH, CLOPTS, 
   or LKOPTS instead of providing options -opt -p, -clopts, or -lkopts .
   Command line options will take precedence over environment variables. 

   Copyright (c) 2015 ADVANCED MICRO DEVICES, INC.

```


## The snack.sh Command 

```
   snack: Generate host-callable "snack" functions for GPU kernels.
          Snack generates the source code and headers for each kernel 
          in the input filename.cl file.  The -c option will compile 
          the source with gcc so you can link with your host application.
          Host applicaton requires no API to use snack functions.

   Usage: snack.sh [ options ] filename.cl

   Options without values:
    -c        Compile generated source code to create .o file
    -hsail    Generate text hsail for manual optimization
    -version  Display version of snack then exit
    -v        Verbose messages
    -vv       Get additional verbose messages from cloc.sh
    -n        Dryrun, do nothing, show commands that would execute
    -h        Print this help message
    -k        Keep temporary files
    -fort     Generate fortran function names
    -noglobs  Do not generate global functions 
    -kstats   Print out kernel statistics (post finalization)
    -str      Depricated, create .o file needed for okra
    -m32      Generate snackwrape in 32-bit mode. If -c, also compile in 32
              bit mode

   Options with values:
    -opt      <LLVM opt>     Default=2, passed to cloc.sh to build HSAIL 
    -gccopt   <gcc opt>      Default=2, gcc optimization for snack wrapper
    -t        <tempdir>      Default=/tmp/snk_$$, Temp dir for files
    -s        <symbolname>   Default=filename 
    -p        <path>         $HSA_LLVM_PATH or <sdir> if HSA_LLVM_PATH not set
                             <sdir> is actual directory of snack.sh 
    -rp       <HSA RT path>  Default=$HSA_RUNTIME_PATH or /opt/hsa
    -o        <outfilename>  Default=<filename>.<ft> 
    -foption  <fnlizer opts> Default=""  Finalizer options
    -hsaillib <hsail filename>  

   Examples:
    snack.sh my.cl              /* create my.snackwrap.c and my.h    */
    snack.sh -c my.cl           /* gcc compile to create  my.o       */
    snack.sh -hsail my.cl       /* create hsail and snackwrap.c      */
    snack.sh -c -hsail my.cl    /* create hsail snackwrap.c and .o   */
    snack.sh -t /tmp/foo my.cl  /* will automatically set -k         */

   You may set environment variables HSA_LLVM_PATH, HSA_RUNTIME_PATH, 
   instead of providing options -p, -rp.
   Command line options will take precedence over environment variables. 

   Copyright (c) 2015 ADVANCED MICRO DEVICES, INC.
```

<A NAME="ReadmeExamples">
# Examples
-------- 

## Example 1: Hello World

This version of cloc supports the SNACK method of writing accelerated 
kernels in c. With SNACK, a host program can directly call the 
accelerated function. 
Here is the c++ source code HelloWorld.cpp using SNACK.
```cpp
//
//    File:  HelloWorld.cpp
//
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#include "hw.h"
int main(int argc, char* argv[]) {
	const char* input = "Gdkkn\x1FGR@\x1FVnqkc";
	size_t strlength = strlen(input);
	char *output = (char*) malloc(strlength + 1);
        SNK_INIT_LPARM(lparm,strlength);
	decode(input,output,lparm);
	output[strlength] = '\0';
	cout << output << endl;
	free(output);
	return 0;
}
```
The c source for the accelerated kernel is in file hw.cl.
```c
/*
    File:  hw.cl 
*/
__kernel void decode(__global char* in, __global char* out) {
	int num = get_global_id(0);
	out[num] = in[num] + 1;
}
```
The host program includes header file "hw.h" that does not exist yet.
The -c option of snack.sh will call the gcc compiler and create 
the object file and the header file.  Without -c you get the 
generated c code hw.snackwrap.c and the header file.  The header file
has function prototypes for all kernels declared in the .cl file.  Use 
this command to compile the hw.cl file with snack.sh.

```
snack.sh -c hw.cl
```

You can now compile and build the binary "HelloWorld" with any c++ compiler.
Here is the command to build HelloWorld with g++. 

```
g++ -o HelloWorld hw.o HelloWorld.cpp -L$HSA_RUNTIME_PATH/lib -lhsa-runtime64 -lelf 

```

Then execute the program as follows.
```
$ ./HelloWorld
Hello HSA World
```

This example and other examples can be found in the CLOC repository in the directory examples/snack.

## Example 2: Manual HSAIL Optimization Process

This version of snack supports a process where a programmer can experiment with
manual updates to HSAIL.  This process has two steps. 

#### Step 1
The first step compiles the .cl file into the object code needed by a SNACK application.
For example, if your kernels are in the file myKernels.cl, then you can run step 1 as follows.
```
   snack.sh -c -hsail myKernels.cl
```
When cloc sees the "-c" option and the "-hsail" option, it will save four files 
in the same directory as myKernels.cl file.  The first two files are always created 
with the -c option. 

 1.  The object file (myKernels.o) to link with your application
 2.  The header file (myKernels.h) for your host code to compile correctly
 3.  The c wrapper code (myKernels.snackwrap.c) needed to recreate the .o file in step 2.
 4.  The HSAIL code (myKernels.hsail) to be manually modified in step 2. 

This is a good time to test your host program before making manual changes to the HSAIL.
BE WARNED, rerunning step 1 will overwrite these files. A common mistake is to rerun
step 1 after you have manually updated your HSAIL code for step 2. This would 
naturally destroy those edits.  

#### Step 2
For step 2, make manual edits to the myKernels.hsail file using your favorite editor.  
You may not change any of the calling arguments or insert new kernels.  This is because 
the generated wrapper from step 1 will be incorrect if you make these types of changes.  
If you want different arguments or new kernels, make those changes in your .cl file and
go back to step 1.  After your manual edits, rebuild the object code from your 
modified hsail as follows. 

```
   snack.sh -c  myKernels.hsail
```
The above will fail if either the myKernels.hsail or myKernels.snackwrap.c file are missing.

## Example 3: Use of -hsaillib by cloc.sh and snack.sh

In version 0.9.5 of cloc.sh and snack.sh , there is an option to provide an HSAIL library.
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
If you want to compile and build the example that uses snack, execute the buildrun.sh script found
in the example directory.  It calls snack as follows. 
```
snack.sh  -v -c  -hsaillib ../../mathdemo_hsaillib/mathdemo_hsaillib.hsail test_hsail_lib.cl
```
If you are just building the hsail for the above kernel, provide the -hsaillib option to cloc.sh
as follows.
```
cloc.sh  -hsail -hsaillib ../../mathdemo_hsaillib/mathdemo_hsaillib.hsail test_hsail_lib.cl
```
Your resulting hsail test_hsail_lib.hsail will include the hsaillib inserted into the correct
position. The generated brig will also include the library. 
