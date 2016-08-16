#!/bin/bash
#
#  snack: Structured No API Compiled Kernels .  Snack is used to
#         generate host-callable functions that launch compiled 
#         GPU and CPU kernels without an API.  Snack generates the
#         wrapper source code for these host-callable functions
#         that embeds compiled kernels into the source. The generated 
#         source code uses the HSA API to launch these kernels 
#         with various synchrnous and asynchronous features.
#         
#         The generated functions are called a "snack" functions
#         An application calls snack functions with the programmer
#         defined name and argument list. An extra argument is 
#         added to the programmer defined argument list to specify
#         the launch parameters. Since the host application directly 
#         calls snack functions and the launch attributes are 
#         specified in a data structure, there is no host API required.
#
#         The snack command requires the cloc.sh tool to generate HSA 
#          Code Object for GPU kernels. Snack is distributed with the 
#         snack github repository.  
#
#  Written by Greg Rodgers  Gregory.Rodgers@amd.com
#
PROGVERSION=1.2.1
#
# Copyright (c) 2016 ADVANCED MICRO DEVICES, INC.  Patent pending.
# 
# AMD is granting you permission to use this software and documentation (if any) (collectively, the 
# Materials) pursuant to the terms and conditions of the Software License Agreement included with the 
# Materials.  If you do not have a copy of the Software License Agreement, contact your AMD 
# representative for a copy.
# 
# You agree that you will not reverse engineer or decompile the Materials, in whole or in part, except for 
# example code which is provided in source code form and as allowed by applicable law.
# 
# WARRANTY DISCLAIMER: THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
# KIND.  AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT 
# LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
# PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE SOFTWARE WILL RUN UNINTERRUPTED OR ERROR-
# FREE OR WARRANTIES ARISING FROM CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK 
# ASSOCIATED WITH THE USE OF THE SOFTWARE IS ASSUMED BY YOU.  Some jurisdictions do not 
# allow the exclusion of implied warranties, so the above exclusion may not apply to You. 
# 
# LIMITATION OF LIABILITY AND INDEMNIFICATION:  AMD AND ITS LICENSORS WILL NOT, 
# UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT, INCIDENTAL, 
# INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF THE SOFTWARE OR THIS 
# AGREEMENT EVEN IF AMD AND ITS LICENSORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH 
# DAMAGES.  In no event shall AMD's total liability to You for all damages, losses, and 
# causes of action (whether in contract, tort (including negligence) or otherwise) 
# exceed the amount of $100 USD.  You agree to defend, indemnify and hold harmless 
# AMD and its licensors, and any of their directors, officers, employees, affiliates or 
# agents from and against any and all loss, damage, liability and other expenses 
# (including reasonable attorneys' fees), resulting from Your use of the Software or 
# violation of the terms and conditions of this Agreement.  
# 
# U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with "RESTRICTED RIGHTS." 
# Use, duplication, or disclosure by the Government is subject to the restrictions as set 
# forth in FAR 52.227-14 and DFAR252.227-7013, et seq., or its successor.  Use of the 
# Materials by the Government constitutes acknowledgement of AMD's proprietary rights in them.
# 
# EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as stated in the 
# Software License Agreement.
# 
function usage(){
/bin/cat 2>&1 <<"EOF" 

   snack: Generate host-callable "snack" functions for GPU kernels.
          Snack generates the source code and headers for each kernel 
          in the input filename.cl file.  The -c option will compile 
          the source with gcc so you can link with your host application.
          Host applicaton requires no API to use snack functions.

   Usage: snack.sh [ options ] filename.cl

   Options without values:
    -c        Compile generated source code to create .o file
    -ll       Tell cloc.sh to generate IR for LLVM steps
    -noqp     Tell cloc.sh to not use quick path
    -noshared Tell cloc.sh to not create shared object.
    -s        Generate gcn assembly (.s) from lld output
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

EOF
   exit 1 
}

DEADRC=12

#  Utility Functions
function do_err(){
   if [ $NEWTMPDIR ] ; then 
      if [ $KEEPTDIR ] ; then 
         cp -rp $TMPDIR $OUTDIR
         [ $VERBOSE ] && echo "#Info:  Temp files copied to $OUTDIR/$TMPNAME"
      fi
      rm -rf $TMPDIR
   else 
      if [ $KEEPTDIR ] ; then 
         [ $VERBOSE ] && echo "#Info:  Temp files kept in $TMPDIR"
      fi 
   fi
   [ $VERBOSE ] && echo "#Info:  Done"
   exit $1
}
function version(){
   echo $PROGVERSION
   exit 0
}
runcmd(){
   THISCMD=$1
   if [ $DRYRUN ] ; then
      echo "$THISCMD"
   else 
      [ $VV ] && echo "$THISCMD"
      $THISCMD
      rc=$?
      if [ $rc != 0 ] ; then 
         echo "ERROR:  The following command failed with return code $rc."
         echo "        $THISCMD"
         do_err $rc
      fi
   fi
}
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

#  --------  The main code starts here -----

#  Argument processing
while [ $# -gt 0 ] ; do 
   case "$1" in 
      -q)               QUIET=true;;
      --quiet)          QUIET=true;;
      -k) 		KEEPTDIR=true;; 
      --keep) 		KEEPTDIR=true;; 
      -n) 		DRYRUN=true;; 
      -c) 		MAKEOBJ=true;;  
      -fort) 		FORTRAN=1;;  
      -noglobs)  	NOGLOBFUNS=1;;  
      -kstats)  	KSTATS=1;;  
      -ll) 		GENLL=true;; 
      -noqp) 		NOQP=true;; 
      -noshared) 	NOSHARED=true;; 
      -s) 		GENASM=true;; 
      -opt) 		LLVMOPT=$2; shift ;; 
      -gccopt) 		GCCOPT=$2; shift ;; 
      -foption) 	FOPTION=$2; shift ;; 
      -s) 		SYMBOLNAME=$2; shift ;; 
      -o) 		OUTFILE=$2; shift ;; 
      -t) 		TMPDIR=$2; shift ;; 
      -path)            CLOC_PATH=$2; shift ;;
      -amdllvm)         AMDLLVM=$2; shift ;;
      -libgcn)          LIBGCN=$2; shift ;;
      -hsart)           HSA_RT=$2; shift ;;
      -m32)		ADDRMODE=32;;
      -mcpu)            MCPU=$2; shift ;;

      -g) 		GEN_DEBUG=true;; 
      -hsaillib)        HSAILLIB=$2; shift ;; 
      -hsail) 		GEN_IL=true;; 
      -brig) 		GEN_BRIG=true;; 

      -h) 		usage ;; 
      -help) 		usage ;; 
      --help) 		usage ;; 
      -version) 	version ;; 
      --version) 	version ;; 
      -v) 		VERBOSE=true;; 
      -vv) 		CLOCVERBOSE=true;; 
      --) 		shift ; break;;
      -*) 		usage ;;
      *) 		break;echo $1 ignored;
   esac
   shift
done

# The above while loop is exited when last string with a "-" is processed
LASTARG=$1
shift

#  Allow output specifier after the cl file
if [ "$1" == "-o" ]; then 
   OUTFILE=$2; shift ; shift; 
fi

if [ ! -z $1 ]; then 
   echo " "
   echo "WARNING:  Snack can only process one .cl file at a time."
   echo "          You can call snack multiple times to get multiple outputs."
   echo "          Argument $LASTARG will be processed. "
   echo "          These args are ignored: $@"
   echo " "
fi

sdir=$(getdname $0)
[ ! -L "$sdir/snack.sh" ] || sdir=$(getdname `readlink "$sdir/snack.sh"`)
CLOC_PATH=${CLOC_PATH:-$sdir}

#  Set Default values
GCCOPT=${GCCOPT:-3}
LLVMOPT=${LLVMOPT:-2}
HSA_RT=${HSA_RT:-/opt/rocm/hsa}

FORTRAN=${FORTRAN:-0};
NOGLOBFUNS=${NOGLOBFUNS:-0};
KSTATS=${KSTATS:-0};
ADDRMODE=${ADDRMODE:-64};
FOPTION=${FOPTION:-"NONE"}

RUNDATE=`date`

filetype=${LASTARG##*\.}
#
# The old snack with brig and hsail is now snackhsail.sh
# The snack.sh will only be for generating code object.
#
if [ "$filetype" == "hsail" ] || [ $GEN_IL ] || [ $GEN_BRIG ] || [ $HSAILLIB ] || [ $GEN_DEBUG ]  ; then 
   echo "ERROR:  The use of brig or hsail in snack.sh is deprecated. Please use"
   echo "        the snackhsail.sh command to generate and embed hsail or brig."
   echo "        This includes the -g option to generate HSAIL debug info"
   exit $DEADRC
fi 

if [ "$filetype" != "cl" ]  ; then 
   echo "ERROR:  $0 requires one argument with file type cl "
   exit $DEADRC 
fi

if [ ! -e "$LASTARG" ]  ; then 
   echo "ERROR:  The file $LASTARG does not exist."
   exit $DEADRC
fi
if [ ! -d $CLOC_PATH ] ; then 
   echo "ERROR:  Missing directory $CLOC_PATH "
   echo "        Set env variable CLOC_PATH or use -p option"
   exit $DEADRC
fi
if [ $MAKEOBJ ] && [ ! -d "$HSA_RT/lib" ] ; then 
   echo "ERROR:  snack.sh -c option needs HSA_RT"
   echo "        Missing directory $HSA_RT/lib "
   echo "        Set env variable HSA_RT or use -hsart option"
   exit $DEADRC
fi
if [ $MAKEOBJ ] && [ ! -f $HSA_RT/include/hsa.h ] ; then 
   echo "ERROR:  Missing $HSA_RT/include/hsa.h"
   echo "        snack.sh requires HSA includes"
   exit $DEADRC
fi

# Parse LASTARG for directory, filename, and symbolname
INDIR=$(getdname $LASTARG)
CLNAME=${LASTARG##*/}
# FNAME has the .cl extension removed, used for symbolname and intermediate filenames
FNAME=`echo "$CLNAME" | cut -d'.' -f1`
SYMBOLNAME=${SYMBOLNAME:-$FNAME}
HSACO_HFILE="${SYMBOLNAME}_hsaco.h"
OTHERCLOCFLAGS=" " 

if [ -z $OUTFILE ] ; then 
#  Output file not specified so use input directory
   OUTDIR=$INDIR
#  Make up the output file name based on last step 
   if [ $MAKEOBJ ] ; then 
      OUTFILE=${FNAME}.o
   else
#     Output is snackwrap.c
      OUTFILE=${FNAME}.snackwrap.c
   fi
else 
#  Use the specified OUTFILE.  Bad idea for snack
   OUTDIR=$(getdname $OUTFILE)
   OUTFILE=${OUTFILE##*/}
fi 

if [ $CLOCVERBOSE ] ; then 
   VERBOSE=true
fi

if [ $GENLL ] ; then
   OTHERCLOCFLAGS="$OTHERCLOCFLAGS -ll"
fi
if [ $NOQP ] ; then
   OTHERCLOCFLAGS="$OTHERCLOCFLAGS -noqp"
fi
if [ $NOSHARED ] ; then
   OTHERCLOCFLAGS="$OTHERCLOCFLAGS -noshared"
fi
if [ $GENASM ] ; then
   OTHERCLOCFLAGS="$OTHERCLOCFLAGS -s"
fi
if [ $AMDLLVM ] ; then
   OTHERCLOCFLAGS="$OTHERCLOCFLAGS -amdllvm $AMDLLVM"
fi
if [ $LIBGCN ] ; then
   OTHERCLOCFLAGS="$OTHERCLOCFLAGS -libgcn $LIBGCN"
fi
if [ $MCPU ] ; then
   OTHERCLOCFLAGS="$OTHERCLOCFLAGS -mcpu $MCPU"
fi

if [ $GEN_DEBUG ] ; then
   export LIBHSAIL_OPTIONS_APPEND="-g -include-source"
   OTHERCLOCFLAGS="$OTHERCLOCFLAGS -g"
fi

TMPNAME="snk-tmp-$$"
TMPDIR=${TMPDIR:-/tmp/$TMPNAME}
if [ -d $TMPDIR ] ; then 
   KEEPTDIR=true
else 
   if [ $DRYRUN ] ; then
      echo "mkdir -p $TMPDIR"
   else
      mkdir -p $TMPDIR
      NEWTMPDIR=true
   fi
fi
# Be sure not to delete the output directory
if [ $TMPDIR == $OUTDIR ] ; then 
   KEEPTDIR=true
fi
if [ ! -d $TMPDIR ] && [ ! $DRYRUN ] ; then 
   echo "ERROR:  Directory $TMPDIR does not exist or could not be created"
   exit $DEADRC
fi 
if [ ! -d $OUTDIR ] && [ ! $DRYRUN ]  ; then 
   echo "ERROR:  The output directory $OUTDIR does not exist"
   exit $DEADRC
fi 

# Snack only needs to compile if -c or -str specified
if [ $MAKEOBJ ] ; then 
   CMD_GCC=`which gcc`
   if [ -z "$CMD_GCC" ] ; then  
      echo "ERROR:  No gcc compiler found."
      exit $DEADRC
   fi
   if [ $ADDRMODE == 32 ] ; then
   	$CMD_GCC = $CMD_GCC -m32
   fi
fi

if [ $MAKEOBJ ] ; then 
   FULLHSACO_HFILE=$TMPDIR/$HSACO_HFILE
   CWRAPFILE=$TMPDIR/$FNAME.snackwrap.c
else
   CWRAPFILE=$OUTDIR/$FNAME.snackwrap.c
   FULLHSACO_HFILE=$OUTDIR/$HSACO_HFILE
fi

if [ $VERBOSE ] ; then 
   echo "#Info:  Version:	snack.sh $PROGVERSION" 
   echo "#Info:  Input File:	"
   echo "#           CL file:	   $INDIR/$CLNAME"
   echo "#Info:  Output Files:"
   if [ $MAKEOBJ ] ; then 
      echo "#           Object:	   $OUTDIR/$OUTFILE"
   else
      echo "#           Wrapper:      $CWRAPFILE"
      echo "#           Include:	   $FULLHSACO_HFILE"
   fi
   echo "#           Headers:	   $OUTDIR/$FNAME.h"
   echo "#Info:  Run date:	$RUNDATE" 
   echo "#Info:  CLOC path:	$CLOC_PATH"
   [ $MAKEOBJ ]  && echo "#Info:  Runtime:	$HSA_RT"
   [ $KEEPTDIR ] && echo "#Info:  Temp dir:	$TMPDIR" 
   [ $MAKEOBJ ]  && echo "#Info:  gcc loc:	$CMD_GCC" 
   echo "#"
fi

rc=0

[ $VERBOSE ] && echo "#Step:  genw  		cl --> $FNAME.snackwrap.c + $FNAME.h ..."
runcmd "$CLOC_PATH/snk_genw.sh $SYMBOLNAME $INDIR/$CLNAME $PROGVERSION $TMPDIR $CWRAPFILE $OUTDIR/$FNAME.h $TMPDIR/updated.cl $FORTRAN $NOGLOBFUNS $KSTATS $ADDRMODE "\"$FOPTION\"""
#  Call cloc to generate hsaco
if [ $CLOCVERBOSE ] ; then 
   OTHERCLOCFLAGS="$OTHERCLOCFLAGS -v"
fi
if [ "$HSAILLIB" != "" ] ; then 
   OTHERCLOCFLAGS="$OTHERCLOCFLAGS -hsaillib $HSAILLIB"
fi
[ $VERBOSE ] && echo "#Step:  cloc.sh		cl --> hsaco ..."
[ $CLOCVERBOSE ] && echo " " && echo "#------ Start cloc.sh output ------"
runcmd "$CLOC_PATH/cloc.sh -t $TMPDIR -k -clopts "-I$INDIR" $OTHERCLOCFLAGS $TMPDIR/updated.cl"
[ $CLOCVERBOSE ] && echo "#------ End cloc.sh output ------" && echo " " 

if [ $GENLL ] ; then
   cp -p $TMPDIR/updated.ll $OUTDIR/$FNAME.ll
   cp -p $TMPDIR/updated.lnkd.ll $OUTDIR/$FNAME.lnkd.ll
   cp -p $TMPDIR/updated.opt.ll $OUTDIR/$FNAME.opt.ll
fi
if [ $GENASM ] ; then
   if [ -f $TMPDIR/updated.s ] ; then cp -p $TMPDIR/updated.s $OUTDIR/$FNAME.s ; fi
fi

[ $VERBOSE ] && echo "#Step:  hexdump		hsaco --> $HSACO_HFILE ..."
if [ $DRYRUN ] ; then 
   echo "echo char _${SYMBOLNAME}_HSA_CodeObjMem[] = { >$HSACO_HFILE " 
   echo hexdump -v -e '"0x" 1/1 "%02X" ","' $TMPDIR/updated.hsaco ">>" $HSACO_HFILE
   echo "echo }; >> $HSACO_HFILE" 
   echo "echo size_t _${SYMBOLNAME}_HSA_CodeObjMemSz = sizeof(_${SYMBOLNAME}_HSA_CodeObjMem); >> $HSACO_HFILE "
else
   echo "char _${SYMBOLNAME}_HSA_CodeObjMem[] = {" > $FULLHSACO_HFILE
   hexdump -v -e '"0x" 1/1 "%02X" ","' $TMPDIR/updated.hsaco >> $FULLHSACO_HFILE
   echo "};" >> $FULLHSACO_HFILE
   echo "size_t _${SYMBOLNAME}_HSA_CodeObjMemSz = sizeof(_${SYMBOLNAME}_HSA_CodeObjMem);" >> $FULLHSACO_HFILE
   if [ ! $MCPU ] ; then
      MCPU=`mymcpu`
      if [ "$MCPU" == "" ] ; then 
         MCPU="fiji"
      fi
   fi
   echo "const char* _${SYMBOLNAME}_MCPU = \"$MCPU\";" >> $FULLHSACO_HFILE
fi

if [ $MAKEOBJ ] ; then 
   [ $VERBOSE ] && echo "#Step:  gcc		snackwrap.c + _hsaco.h --> $OUTFILE  ..."
   runcmd "$CMD_GCC -O$GCCOPT -I$TMPDIR -I$INDIR -I$CLOC_PATH/../include -I$HSA_RT/include -o $OUTDIR/$OUTFILE -c $CWRAPFILE"
   if [ $KSTATS == 1 ] ; then 
      runcmd "$CMD_GCC -o $TMPDIR/kstats -O$GCCOPT -I$TMPDIR -I$INDIR -I$CLOC_PATH/../include -I$HSA_RT/include $OUTDIR/$OUTFILE $TMPDIR/kstats.c -L$HSA_RT/lib -lhsa-runtime64"
      runcmd "$TMPDIR/kstats"
   fi 

fi

# cleanup
do_err 0

exit 0
