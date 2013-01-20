#!/bin/bash
#
#

export ARCH=arm
export CROSS_COMPILE=../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
make msm7630-perf_j_masai_e_tp10_defconfig
make -j8 CONFIG_DEBUG_SECTION_MISMATCH=y ARCH=arm 
