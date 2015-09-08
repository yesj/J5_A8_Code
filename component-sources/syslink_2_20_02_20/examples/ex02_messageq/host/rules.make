BUILD_PATH    = $(SYSLINK_INSTALL_DIR)/../..
##dingding, 20/01/2014, no need to modify manually by everyone, comment off CROSS_COMPILE,CC, modify BUILD_PATH
# CROSS_COMPILE = /home/user/arm-2009q1/bin/arm-none-linux-gnueabi-
KERNEL_DIR    = $(BUILD_PATH)/board-support/linux-2.6.37-psp04.07.00.02
##TGTFS_PATH    = $(BUILD_PATH)/filesys
GSDK_ROOT     = $(BUILD_PATH)/component-sources/graphics-sdk_4.07.00.01
##DVSDK_ROOT    = $(BUILD_PATH)/../dvsdk/dvsdk_3_01_00_04

# no need to touch from here
GLES_DIR      = $(GSDK_ROOT)/include/OGLES
GLES2_DIR     = $(GSDK_ROOT)/include/OGLES2
GSDK_KM_DIR   = $(GSDK_ROOT)/GFX_Linux_KM
##X11_DIR       = $(GSDK_ROOT)/gfx_rel/freedesktop/kdrive/usr/X11R6_SGX
##CMEM_DIR      = $(DVSDK_ROOT)/linuxutils_2_25_00_03/packages/ti/sdo/linuxutils/cmem

# CC = $(CROSS_COMPILE)gcc
