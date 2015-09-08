#
#  Copyright (c) 2012, Texas Instruments Incorporated
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  *  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#  *  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  *  Neither the name of Texas Instruments Incorporated nor the names of
#     its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#
#  ======== Makefile ========
#
##liuxu, 8/21/2013, add src and lib for mpeg4 encoder.

##liuxu, 8/28/2013, add for GFX demo.
include ./rules.make

##liuxu, 8/28/2013, add for GFX demo. for GL20
##dingding, 20/01/2014, no need to modify manually rules.make by everyone, make automatical on svn, remove :
GLES_DIR = $(GLES2_DIR)

##liuxu, 8/28/2013. 
srcs = main_host.c App.c ParaMP4Enc.c ParaParser.c CallBack.c common.c

srcs_cpp = customer_folder/111.cpp

EXBASE = ..
include $(EXBASE)/products.mak
-include $(addprefix bin/$(PROFILE)/obj/,$(patsubst %.c,%.ov5T.dep,$(srcs)))

#liuxu, 10/3/2013.
-include $(addprefix bin/$(PROFILE)/obj/,$(patsubst %.cpp,%.ov5T.dep,$(srcs_cpp)))

objs = $(addprefix bin/$(PROFILE)/obj/,$(patsubst %.c,%.ov5T,$(srcs)))
$(warning $(objs))

##liuxu, 10/3/2013, for compile CPP.
objs_cpp = $(addprefix bin/$(PROFILE)/obj/,$(patsubst %.cpp,%.ov5T,$(srcs_cpp)))

$(warning $(objs_cpp))

libs = $(SYSLINK_INSTALL_DIR)/packages/ti/syslink/lib/syslink.a_$(PROFILE) 
##dingding libmpeg4enc_arm.a

PKGPATH := $(SYSLINK_INSTALL_DIR)/packages
PKGPATH := $(PKGPATH)+$(BIOS_INSTALL_DIR)/packages
PKGPATH := $(PKGPATH)+$(IPC_INSTALL_DIR)/packages

##liuxu, 8/28/2013, add for GFX demo.
##liuxu, 01/10/2014, +$(KERNEL_DIR)/include+$(KERNEL_DIR)/arch/arm/include/+$(KERNEL_DIR)/arch/arm/plat-omap/include/
PKGPATH := $(PKGPATH)+$(GSDK_KM_DIR)/include4+$(GSDK_KM_DIR)/services4/include+$(GLES_DIR)+$(GSDK_KM_DIR)/services4/3rdparty/bufferclass_ti+$(KERNEL_DIR)/include+$(KERNEL_DIR)/arch/arm/include/+$(KERNEL_DIR)/arch/arm/plat-omap/include/

all:
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	$(MAKE) PROFILE=debug app_host
	$(MAKE) PROFILE=release app_host

help:
	@$(ECHO) "make                  # build executables"
	@$(ECHO) "make clean            # clean everything"

install:
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	@$(MKDIR) $(EXEC_DIR)/debug
	$(CP) bin/debug/app_host $(EXEC_DIR)/debug
	@$(MKDIR) $(EXEC_DIR)/release
	$(CP) bin/release/app_host $(EXEC_DIR)/release

clean::
	$(RMDIR) bin/debug/obj/customer_folder/*.ov5T*
	$(RMDIR) bin/debug/obj/*.ov5T*

        ##liuxu, 11/19/2013, clean release folder as well.
	$(RMDIR) bin/release/obj/customer_folder/*.ov5T*
	$(RMDIR) bin/release/obj/*.ov5T*


#
#  ======== rules ========
#
app_host: bin/$(PROFILE)/app_host
bin/$(PROFILE)/app_host: $(objs) $(objs_cpp) $(libs)
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

	
bin/$(PROFILE)/obj/%.ov5T: %.c
	$(ECHO) "#liuxuliuxu"
	@$(ECHO) "# Making $@ ..."
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<
	
#liuxu, 10/3/2013, rule for cpp.	
bin/$(PROFILE)/obj/%.ov5T: %.cpp
	$(ECHO) "#liuxuliuxu"
	@$(ECHO) "# Making $@ ..."
	$(CXX) $(CPPFLAGS) $(CFLAGS) -o $@ $<


#  ======== install validation ========
ifeq (install,$(MAKECMDGOALS))
ifeq (,$(EXEC_DIR))
$(error must specify EXEC_DIR)
endif
endif

#  ======== toolchain macros ======== liuxu, 10/3/2013, change to g++.
CC = $(CGT_ARM_PREFIX)gcc -c -MD -MF $@.dep -march=armv5t

CXX = $(CGT_ARM_PREFIX)g++ -c -MD -MF -march=armv5t

AR = $(CGT_ARM_PREFIX)ar cr
LD = $(CGT_ARM_PREFIX)g++

CPPFLAGS = -D_REENTRANT -Dxdc_target_name__=GCArmv5T \
    -Dxdc_target_types__=gnu/targets/arm/std.h

##liuxu, 8/28/2013
CFLAGS =  -Wall -ffloat-store  -fPIC -Wunused -Dfar= $(CCPROFILE_$(PROFILE)) -DGLES_20\
    -I. $(addprefix -I,$(subst +, ,$(PKGPATH))) 

##liuxu, 8/28/2013
LDFLAGS = -L$(GSDK_ROOT)/gfx_dbg_es6.x/ -Wl,--rpath=$(GSDK_ROOT)/gfx_dbg_es6.x $(LDPROFILE_$(PROFILE)) -Wall -Wl,-Map=$@.map

##liuxu, 8/21/2013.
##liuxu, 8/28/2013.
LDLIBS = -lpthread -lc -lrt -lEGL -lGLESv2

CCPROFILE_debug = -ggdb -D DEBUG
CCPROFILE_release = -O3 -D NDEBUG

LDPROFILE_debug = -ggdb
LDPROFILE_release = -O3

#  ======== standard macros ========
ifneq (,$(wildcard $(XDC_INSTALL_DIR)/bin/echo.exe))
    # use these on Windows
    CP      = $(XDC_INSTALL_DIR)/bin/cp
    ECHO    = $(XDC_INSTALL_DIR)/bin/echo
    MKDIR   = $(XDC_INSTALL_DIR)/bin/mkdir -p
    RM      = $(XDC_INSTALL_DIR)/bin/rm -f
    RMDIR   = $(XDC_INSTALL_DIR)/bin/rm -rf
else
    # use these on Linux
    CP      = cp
    ECHO    = echo
    MKDIR   = mkdir -p
    RM      = rm -f
    RMDIR   = rm -rf
endif

#  ======== create output directories ========
ifneq (clean,$(MAKECMDGOALS))
ifneq (,$(PROFILE))
ifeq (,$(wildcard bin/$(PROFILE)/obj))
    $(shell $(MKDIR) -p bin/$(PROFILE)/obj)
endif
endif
endif
