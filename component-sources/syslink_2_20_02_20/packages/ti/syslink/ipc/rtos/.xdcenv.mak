#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/chandler/Dev_J5ECO/component-sources/ipc_1_24_03_32/packages;/home/chandler/Dev_J5ECO/component-sources/bios_6_33_05_46/packages;/home/chandler/Dev_J5ECO/component-sources/syslink_2_20_02_20/packages
override XDCROOT = /home/chandler/Dev_J5ECO/component-sources/xdctools_3_23_03_53
override XDCBUILDCFG = /home/chandler/Dev_J5ECO/component-sources/syslink_2_20_02_20/packages/_config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = DEVICE=TI811X CGT_C674_ELF_DIR=/home/chandler/Dev_J5ECO/dsp-devkit/cgt6x_7_3_4 CGT_M3_ELF_DIR= DEVICE_VARIANT=TI811X TI81XXDSP_DMTIMER_FREQ=20000000
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/chandler/Dev_J5ECO/component-sources/ipc_1_24_03_32/packages;/home/chandler/Dev_J5ECO/component-sources/bios_6_33_05_46/packages;/home/chandler/Dev_J5ECO/component-sources/syslink_2_20_02_20/packages;/home/chandler/Dev_J5ECO/component-sources/xdctools_3_23_03_53/packages;../../../..
HOSTOS = Linux
endif
