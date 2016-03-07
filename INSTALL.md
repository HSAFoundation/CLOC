Cloc Install Instructions
===============================

Warning.  These instructions are for CLOC 1.0 (March 2016 Update) .

This set of instructions can be used to install a comprehensive HSA software stack and the Cloc utility for Ubuntu.  In addition to Linux, you must have an HSA compatible system such as a Kaveri processor, a Carrizo processor, or a fiji card. There are four steps to this process:

- [1. Prepare for Upgrade](#Prepare)
- [2. Install Linux Kernel](#Boot)
- [3. Install HSA Software](#Install)
- [4. Install Optional Infiniband Software](#Infiniband)

<A Name="Prepare">
1. Prepare System
=================

## Install Ubuntu 14.04 LTS

Make sure Ubuntu 14.04 LTS 64-bit version has been installed.  Ubunutu 14.04 is also known as trusty.  We recommend the server package set.  The utica version of ubuntu (14.10) has not been tested with HSA.  Then install these dependencies:
```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install gcc-4.9
sudo apt-get install g++-4.9
sudo apt-get install gfortran-4.9
sudo apt-get install git
sudo apt-get install make
sudo apt-get install g++
sudo apt-get install gcc
sudo apt-get install gfortran
sudo apt-get install libelf
sudo apt-get install libtinfo-dev
sudo apt-get install re2c
sudo apt-get install libbsd-dev
sudo apt-get install build-essential 
```

## Uninstall Infiniband

If you have Infiniband installed, uninstall the MLNX_OFED packages. 
```
mount the appropriate MLNX_OFED iso
/<mount point>/uninstall.sh
```


<A Name="Boot">
2. Install Linux Kernel Drivers 
===============================

## Install HSA Linux Kernel Drivers 

These instructions are for Boltzman release of HSA 1.0.

Execute these commands:

```
cd ~/git
git clone https://github.com/RadeonOpenCompute/ROCK-Kernel-Driver
sudo dpkg -i ROCK-Kernel-Driver/packages/ubuntu/*.deb
```

## Reboot System

```
sudo reboot
```

<A Name="Install">
3. Install HSA Software
=======================

## Install Boltzman 1.0 Runtime

```
mkdir ~/git
cd ~/git
git clone https://github.com/RadeonOpenCompute/ROCR-Runtime.git
cd ROCR-Runtime/packages/ubuntu
sudo dpkg -i hsa-runtime-dev*.deb
```

## Install CLOC 1.0

```
cd ~/git
git clone -b 1.0 https://github.com/HSAFoundation/cloc
cd cloc/packages/ubuntu
sudo dpkg -i *.deb
```

## Test if HSA is Active.

Use "kfd_check_installation.sh" in cloc/bin to verify installation.

``` 
cd ~/git/cloc/bin
./kfd_check_installation.sh
``` 

The output of above command should look like this.

```
Kaveri detected:............................Yes
Kaveri type supported:......................Yes
Radeon module is loaded:....................Yes
KFD module is loaded:.......................Yes
AMD IOMMU V2 module is loaded:..............Yes
KFD device exists:..........................Yes
KFD device has correct permissions:.........Yes
Valid GPU ID is detected:...................Yes
Can run HSA.................................YES
```

If it does not detect a valid GPU ID (last two entries are NO), it is possible that you need to turn the IOMMU on in the firmware.  Reboot your system and interrupt the boot process to get the firmware screen. Then find the menu to turn on IOMMU and switch from disabled to enabled.  Then select "save and exit" to boot your system.  Then rerun the test script.


## Set LD_LIBRARY_PATH

```
export LD_LIBRARY_PATH=/opt/hsa/lib
```

## Test snack and cloc examples

We recommend that cloc.sh, snack,sh, and printhsail be available in your path.  You can symbolically link them or add to PATH as follows:
```
#
# Either put /opt/amd/cloc/bin in your PATH 
export PATH=$PATH:/opt/amd/cloc/bin
#
# OR symbolic link cloc.sh and snack.sh to system path
sudo ln -sf /opt/amd/cloc/bin/cloc.sh /usr/local/bin/cloc.sh
sudo ln -sf /opt/amd/cloc/bin/snack.sh /usr/local/bin/snack.sh
sudo ln -sf /opt/amd/cloc/bin/snackhsail.sh /usr/local/bin/snackhsail.sh
sudo ln -sf /opt/amd/cloc/bin/printhsail /usr/local/bin/printhsail
```
Now you can test the snack and cloc eamples
```
cd ~/git/cloc/examples/snack/helloworld
./buildrun.sh
cd ~/git/cloc/examples/hsa/vector_copy_codeobject
make
make test
```

## Install HCC Compiler from Multicoreware  (OPTIONAL)

```
mkdir -p $HOME/debs
cd $HOME/debs
wget https://bitbucket.org/multicoreware/cppamp-driver-ng/downloads/hcc-0.8.1545-15f927e-ee0f474-183de0b-Linux.deb
sudo dpkg -i hcc-0.8.1545-15f927e-ee0f474-183de0b-Linux.deb
```

## Install Development GCC6 OpenMP for HSA Compiler (OPTIONAL)

```
cd $HOME/git
git clone -b master https://github.com/HSAFoundation/hsa-openmp-gcc-amd
sudo rsync -av --exclude .git $HOME/git/hsa-openmp-gcc-amd/usr/local/hsagccver  /usr/local
sudo rm -f /usr/local/hsagcc
sudo rsync -av $HOME/git/hsa-openmp-gcc-amd/usr/local/hsagcc  /usr/local
```

## Install HSAIL Debugger and Profiler 

Instructions coming soon. 


<A Name="Infiniband">
4. Optional Infiniband Install 
==============================

## Download OFED

Go to the Mellanox site and download the following MLNX_OFED iso package:
```
http://www.mellanox.com/page/products_dyn?product_family=26
2.4-1.0.4/Ubuntu/Ubuntu 14.10/x86_86/MLNX_OFED*.iso     
```
<b>Ubuntu 14.10</b> is the one that needs to be downloaded to be able to build and install with the 3.17 and 3.19 kernels

## Install MLNX_OFED 

```
Mount the MLNX_OFED iso for Ubuntu 14.10
sudo /<mount point>/mlnxofedinstall --skip-distro-check
```

## Setup Infiniband IP Networking

Edit /etc/network/interfaces to setup IPoIB (ib0)

## Run Subnet Manager

On a couple of the systems, opensm should be run
```
sudo /etc/init.d/opensmd start
```

## Load Kernel Components

Load the IB/RDMA related kernel components
```
sudo /etc/init.d/openibd restart
```

## Verify Install

Run this command
```
lsmod | egrep .ib_|mlx|rdma.  
```
It should show 14 or more IB-related modules loaded.

