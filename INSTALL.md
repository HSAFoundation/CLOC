Cloc Install Instructions
===============================

These instructions are for CLOC 1.0.10 (April 2016 Update). CLOC now installs from the ROCM apt server.  Please remove old packages that you installed with dpkg -i before using the new apt server.   These include amdcloc, libamdgcn, amdllvm, amdllvmbin, hlc, libhsakmt, hsa-runtime-dev, and hcc.  Use the "dpkg -P" command to remove these packages. 

This set of instructions can be used to install a comprehensive HSA software stack and the Cloc utility for Ubuntu.  In addition to Linux, you must have an HSA compatible system such as a Kaveri processor, a Carrizo processor, or a fiji card. There are four steps to this process:

- [1. Prepare for Upgrade](#Prepare)
- [2. Install ROCM Software](#Boot)
- [3. Install and Test CLOC](#Install)
- [4. Optional Install of Infiniband Software](#Infiniband)

<A Name="Prepare">
1. Prepare System
=================

## Install Ubuntu 14.04 LTS

Make sure Ubuntu 14.04 LTS 64-bit version has been installed.  Ubunutu 14.04 is also known as trusty.  We recommend the server package set.  The utica version of ubuntu (14.10) has not been tested with HSA.  

After you install Ubuntu, add two additional repositories with these root-authorized commands:
```
sudo su - 
add-apt-repository ppa:ubuntu-toolchain-r/test
wget -qO - http://packages.amd.com/rocm/apt/debian/rocm.gpg.key | apt-key add -
echo 'deb [arch=amd64] http://packages.amd.com/rocm/apt/debian/ trusty main' > /etc/apt/sources.list.d/rocm.list
apt-get update
apt-get upgrade
```

## Uninstall Infiniband

If you have Infiniband installed, uninstall the MLNX_OFED packages. 
```
mount the appropriate MLNX_OFED iso
/<mount point>/uninstall.sh
```

<A Name="Boot">
2. Install ROCM Software
========================

## Install HSA Linux Kernel Drivers, HSA Runtime, and HCC

Install rocm software packages and reboot your system with these commands:

```
sudo apt-get install rocm
sudo reboot
```

<A Name="Install">
3. Install and Test CLOC 
---=====================

## Install the amdcloc Package

Execute this command:

```
sudo apt-get install amdcloc
```

## Test if HSA is Active.

Use "kfd_check_installation.sh" to verify the HSA installation. Execute this command:

``` 
/opt/rocm/cloc/bin/kfd_check_installation.sh
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

## Test snack and cloc examples

Test the snack and cloc examples as follows.
```
cp -rp /opt/rocm/cloc/examples/snack/helloworld /tmp/.
cd /tmp/helloworld
./buildrun.sh

cp -rp /opt/rocm/cloc/examples/hsa/vector_copy_codeobject  /tmp/.
cd /tmp/vector_copy_codeobject
make
make test
```

## Install Development GCC6 OpenMP for HSA Compiler (OPTIONAL)

The HSA plugin for GCC 6 is currently experimental.  We build versions of the development compiler for testing the HSA plugin.   These can be downloaded and installed as follows. 

```
cd $HOME/git
git clone -b master https://github.com/HSAFoundation/hsa-openmp-gcc-amd
sudo rsync -av --exclude .git $HOME/git/hsa-openmp-gcc-amd/usr/local/hsagccver  /usr/local
sudo rm -f /usr/local/hsagcc
sudo rsync -av $HOME/git/hsa-openmp-gcc-amd/usr/local/hsagcc  /usr/local
```
In the future, we will add add debian packages to the apt server for gcc6.

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

