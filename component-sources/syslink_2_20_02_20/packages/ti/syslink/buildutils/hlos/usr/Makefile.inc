#   
#   @file   Makefile.inc
#
#   @brief  Base makefile for building the user-side of SysLink Library
#           and Sample Application.
#
#
#   ============================================================================
#
#   Copyright (c) 2008-2012, Texas Instruments Incorporated
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#   
#   *  Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#   
#   *  Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#   
#   *  Neither the name of Texas Instruments Incorporated nor the names of
#      its contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#   
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#   Contact information for paper mail:
#   Texas Instruments
#   Post Office Box 655303
#   Dallas, Texas 75265
#   Contact information: 
#   http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
#   DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
#   ============================================================================
#   

# ---------------------------------------------------------------------------- #
# Defines: App build can override.                                             #
# ---------------------------------------------------------------------------- #
# Platform defines
ifeq ("$(SYSLINK_PLATFORM)", "")
# Default value
SYSLINK_PLATFORM := TI81XX
endif # ifeq ("$(SYSLINK_PLATFORM)", "")

ifeq ("$(SYSLINK_PLATFORM)", "OMAP3530")
COMPILE_FLAGS += -DSYSLINK_PLATFORM_OMAP3530
endif # ifeq ("$(SYSLINK_PLATFORM)", "OMAP3530")

ifeq ("$(SYSLINK_PLATFORM)", "TI81XX")
COMPILE_FLAGS += -DSYSLINK_PLATFORM_TI81XX

#If being built for EZSDK, this define needs to be provided to the build
#system.
ifeq ("$(SYSLINK_SDK)", "EZSDK")
COMPILE_FLAGS += -DSYSLINK_SDK_EZSDK
endif # ifeq ("$(SYSLINK_SDK)", "EZSDK")

#COMPILE_FLAGS += -DTI81XX_DSP
COMPILE_FLAGS += -DTI81XX_VIDEOM3
#COMPILE_FLAGS += -DTI81XX_VPSSM3
endif # ifeq ("$(SYSLINK_PLATFORM)", "TI81XX")

ifeq ("$(SYSLINK_PLATFORM)", "OMAPL1XX")
COMPILE_FLAGS += -DSYSLINK_PLATFORM_OMAPL1XX
endif # ifeq ("$(SYSLINK_PLATFORM)", "OMAPL1XX")

# Operating System
BUILDOS_LINUX_OR_ANDROID :=

ifeq ("$(SYSLINK_BUILDOS)", "Linux")
SYSLINK_BUILDOS_LINUX := 1
BUILDOS_LINUX_OR_ANDROID := 1
SYSLINK_BUILDOS_DIR := Linux
endif # ifeq ("$(SYSLINK_BUILDOS_LINUX)", "")

ifeq ("$(SYSLINK_BUILDOS_LINUX)", "1")
COMPILE_FLAGS += -DSYSLINK_BUILDOS_LINUX
endif # ifeq ("$(SYSLINK_BUILDOS)", "Linux")

ifeq ("$(SYSLINK_BUILDOS)", "Qnx")
COMPILE_FLAGS += -DSYSLINK_BUILDOS_QNX
SYSLINK_BUILDOS_DIR := Qnx
endif # ifeq ("$(SYSLINK_BUILDOS)", "Qnx")

# Optimize build flag
ifeq ("$(SYSLINK_BUILD_OPTIMIZE)", "")
SYSLINK_BUILD_OPTIMIZE :=
endif # ifeq ("$(SYSLINK_BUILD_OPTIMIZE)", "")
ifeq ("$(SYSLINK_BUILD_OPTIMIZE)", "1")
COMPILE_FLAGS += -DSYSLINK_BUILD_OPTIMIZE
endif # ifeq ("$(SYSLINK_BUILD_OPTIMIZE)", "1")

# Debug build
ifeq ("$(SYSLINK_BUILD_DEBUG)", "")
SYSLINK_BUILD_DEBUG := 1
endif # ifeq ("$(SYSLINK_BUILD_DEBUG)", "")
ifeq ("$(SYSLINK_BUILD_DEBUG)", "1")
COMPILE_FLAGS += -DSYSLINK_BUILD_DEBUG
endif # ifeq ("$(SYSLINK_BUILD_DEBUG)", "1")

# Trace enable
ifeq ("$(SYSLINK_TRACE_ENABLE)", "")
SYSLINK_TRACE_ENABLE := 1
endif # ifeq ("$(SYSLINK_TRACE_ENABLE)", "")
ifeq ("$(SYSLINK_TRACE_ENABLE)", "1")
COMPILE_FLAGS += -DSYSLINK_TRACE_ENABLE
endif # ifeq ("$(SYSLINK_TRACE_ENABLE)", "1")

# For Netra simulator
ifeq ("$(SYSLINK_PLATFORM)", "TI81XX")
#COMPILE_FLAGS += -DNETRA_SIMULATOR    # Build is for Netra simulator
endif # ifeq ("$(SYSLINK_PLATFORM)", "TI81XX")

COMPILE_FLAGS += -DSYSLINK_BUILD_HLOS
# ---------------------------------------------------------------------------- #
# Toolchain dir: App build can override.                                       #
# ---------------------------------------------------------------------------- #
ifeq ("$(TOOLCHAIN_PREFIX)", "")
ifeq ("$(SYSLINK_PLATFORM)", "OMAP3530")
TOOLCHAIN_PREFIX := /opt/DSPLINK/arm-2009q1-203/bin/arm-none-linux-gnueabi-
endif # ifeq ("$(SYSLINK_PLATFORM)", "OMAP3530")

ifeq ("$(SYSLINK_PLATFORM)", "TI81XX")
TOOLCHAIN_PREFIX := /opt/DSPLINK/arm-2009q1-203/bin/arm-none-linux-gnueabi-
endif # ifeq ("$(SYSLINK_PLATFORM)", "TI81XX")

ifeq ("$(SYSLINK_PLATFORM)", "OMAPL1XX")
TOOLCHAIN_PREFIX := /opt/DSPLINK/arm-2009q1-203/bin/arm-none-linux-gnueabi-
endif # ifeq ("$(SYSLINK_PLATFORM)", "OMAPL1XX")

endif # ifeq ("$(TOOLCHAIN_PREFIX)", "")

#################################ONLY FOR LINUX BUILD###########################
ifeq ("$(SYSLINK_BUILDOS)", "Linux")
# ---------------------------------------------------------------------------- #
# Flags                                                                        #
# ---------------------------------------------------------------------------- #
CC               := $(TOOLCHAIN_PREFIX)gcc
LD               := $(TOOLCHAIN_PREFIX)ld
AR               := $(TOOLCHAIN_PREFIX)ar

CFLAGS           += -Wall -ffloat-store -D_REENTRANT
COPTIMIZE        := -O3

ifeq ("$(GCC_TOOL_WRAPPER)", "1")
ifeq ("$(TOOLCHAIN_PREFIX)", "c6x-coff-linux-")
LINKFLAGS        := -Wl,-ar -pthread
else
LINKFLAGS        :=  --static -dsbt -pthread
CFLAGS           += -gcc -dsbt -static
endif # ifeq ("$(TOOLCHAIN_PREFIX)", "c6x-coff-linux-")
else
LINKFLAGS        := -lpthread -lc
endif # ifeq ("$(GCC_TOOL_WRAPPER)", "1")

endif # ifeq ("$(SYSLINK_BUILDOS)", "Linux")

# ---------------------------------------------------------------------------- #
# Source files                                                                 #
# ---------------------------------------------------------------------------- #
OBJECT :=
# IPC
-include $(SYSLINK_ROOT)/ti/syslink/ipc/hlos/Makefile.inc
-include $(SYSLINK_ROOT)/ti/syslink/ipc/hlos/usr/Makefile.inc
-include $(SYSLINK_ROOT)/ti/syslink/ipc/hlos/usr/$(SYSLINK_BUILDOS_DIR)/Makefile.inc
# OSAL
-include $(SYSLINK_ROOT)/ti/syslink/osal/posix/Makefile.inc
# Utility
-include $(SYSLINK_ROOT)/ti/syslink/utils/hlos/Makefile.inc
-include $(SYSLINK_ROOT)/ti/syslink/utils/common/Makefile.inc
-include $(SYSLINK_ROOT)/ti/syslink/utils/hlos/usr/Makefile.inc
-include $(SYSLINK_ROOT)/ti/syslink/utils/hlos/usr/$(SYSLINK_BUILDOS_DIR)/Makefile.inc
-include $(SYSLINK_ROOT)/ti/syslink/utils/hlos/usr/osal/$(SYSLINK_BUILDOS_DIR)/Makefile.inc
# Proc Manager
-include $(SYSLINK_ROOT)/ti/syslink/procMgr/hlos/usr/Makefile.inc
-include $(SYSLINK_ROOT)/ti/syslink/procMgr/hlos/usr/$(SYSLINK_BUILDOS_DIR)/Makefile.inc
# Syslink configuration
-include $(SYSLINK_ROOT)/ti/syslink/cfg/Makefile.inc


# ---------------------------------------------------------------------------- #
# Header files                                                                 #
# Change the relative path environment according to sample directory location  #
# ---------------------------------------------------------------------------- #
COMPILE_FLAGS += -I$(SYSLINK_ROOT)
#COMPILE_FLAGS += -I$(SYSLINK_ROOT)/ti/syslink/inc
#COMPILE_FLAGS += -I$(SYSLINK_ROOT)/ti/syslink/inc/usr
#COMPILE_FLAGS += -I$(SYSLINK_ROOT)/ti/syslink/inc/usr/$(SYSLINK_BUILDOS_DIR)
# COMPILE_FLAGS += -I$(SYSLINK_ROOT)/ti/syslink/samples/hlos
COMPILE_FLAGS += $(addprefix -I,$(subst ;, ,$(SYSLINK_PKGPATH)))

COBJS       = $(addprefix $(OBJS_DIR)/, $(addsuffix .$(OBJSUFFIX), $(notdir $(basename $(CSRCS)))))

DCOBJS   = $(addprefix $(OBJS_DIR)/debug/, $(addsuffix .$(OBJSUFFIX), $(notdir $(basename $(CSRCS)))))
RCOBJS   = $(addprefix $(OBJS_DIR)/release/, $(addsuffix .$(OBJSUFFIX), $(notdir $(basename $(CSRCS)))))

################################################################################
## Quiet build
################################################################################
ifndef V
Q           := @
endif

######################## COMMON FOR LINUX AND ANDROID BUILDS ###################
ifneq ("$(BUILDOS_LINUX_OR_ANDROID)", "")
################################################################################
## Sample Application Library Dependency
################################################################################
ifdef APP_DEP_LIB
APP_DEP_LIB_DEBUG   := $(addsuffix _debug,   $(APP_DEP_LIB))
APP_DEP_LIB_RELEASE := $(addsuffix _release, $(APP_DEP_LIB))
endif

################################################################################
## SysLink sources Build rules
################################################################################
# Compile template for source files
define Compile_template
# Rule to build standard
$(OBJS_DIR)/$(notdir $(basename $(1)).$(OBJSUFFIX)): $(basename $(1)).c
	@echo Compiling: $(basename $(1)).c
ifeq ("$(GCC_TOOL_WRAPPER)", "1")
	$(Q)$(CC) $(CFLAGS) $(COPTIMIZE) $(COMPILE_FLAGS) -D DEBUG -c -o $(OBJS_DIR)/$(notdir $(basename $(1)).$(OBJSUFFIX)) $(basename $(1)).c
else
	$(Q)$(CC) $(CFLAGS) $(COPTIMIZE) $(COMPILE_FLAGS) -ggdb -D DEBUG -c -o $(OBJS_DIR)/$(notdir $(basename $(1)).$(OBJSUFFIX)) $(basename $(1)).c
endif

# Rule to build debug
$(OBJS_DIR)/debug/$(notdir $(basename $(1)).$(OBJSUFFIX)): $(basename $(1)).c
	@echo Compiling: $(basename $(1)).c
ifeq ("$(GCC_TOOL_WRAPPER)", "1")
	$(Q)$(CC) $(CFLAGS) $(COMPILE_FLAGS) -O0 -D DEBUG -c -o $(OBJS_DIR)/debug/$(notdir $(basename $(1)).$(OBJSUFFIX)) $(basename $(1)).c
else
	$(Q)$(CC) $(CFLAGS) $(COMPILE_FLAGS) -O0 -ggdb -D DEBUG -c -o $(OBJS_DIR)/debug/$(notdir $(basename $(1)).$(OBJSUFFIX)) $(basename $(1)).c
endif

# Rule to build release
$(OBJS_DIR)/release/$(notdir $(basename $(1)).$(OBJSUFFIX)): $(basename $(1)).c
	@echo Compiling: $(basename $(1)).c
ifeq ("$(GCC_TOOL_WRAPPER)", "1")
	$(Q)$(CC) $(CFLAGS) $(COPTIMIZE) $(COMPILE_FLAGS) -D NDEBUG -c -o $(OBJS_DIR)/release/$(notdir $(basename $(1)).$(OBJSUFFIX)) $(basename $(1)).c
else
	$(Q)$(CC) $(CFLAGS) $(COPTIMIZE) $(COMPILE_FLAGS) -D NDEBUG -c -o $(OBJS_DIR)/release/$(notdir $(basename $(1)).$(OBJSUFFIX)) $(basename $(1)).c
endif
endef

# Create rules for compiling C files
$(foreach prog, $(CSRCS),  $(eval $(call Compile_template,$(prog))))

# Rule to build libraries
$(LIB_DIR)/$(SYSLINK_LIB):         $(COBJS)
	@echo Building library $@
	$(Q)$(LD) $(COBJS) -r -o $@
$(LIB_DIR)/$(SYSLINK_LIB)_debug:   $(DCOBJS)
	@echo Building library $@
	$(Q)$(LD) $(DCOBJS) -r -o $@
$(LIB_DIR)/$(SYSLINK_LIB)_release: $(RCOBJS)
	@echo Building library $@
	$(Q)$(LD) $(RCOBJS) -r -o $@

# Rule to build acrhives
$(LIB_DIR)/$(SYSLINK_ARCHIVE):         $(COBJS)
	@echo Archiving $@
	$(Q)$(AR) -rs $@ $(COBJS)
$(LIB_DIR)/$(SYSLINK_ARCHIVE)_debug:   $(DCOBJS)
	@echo Archiving $@
	$(Q)$(AR) -rs $@ $(DCOBJS)
$(LIB_DIR)/$(SYSLINK_ARCHIVE)_release: $(RCOBJS)
	@echo Archiving $@
	$(Q)$(AR) -rs $@ $(RCOBJS)

endif # ifneq ("$(BUILDOS_LINUX_OR_ANDROID)", "")


######################## COMMON FOR LINUX AND ANDROID BUILDS ###################
## Samples build rules common to Linux and Android
################################################################################
ifneq ("$(BUILDOS_LINUX_OR_ANDROID)", "")
SAMPLES_OBJS_DIR = $(SAMPLES_DIR)/$(SAMPLE)
SAMPLES_OBJS_DIR_DBG = $(SAMPLES_OBJS_DIR)/debug
SAMPLES_OBJS_DIR_REL = $(SAMPLES_OBJS_DIR)/release
SAMPLE_DCOBJS   = $(addprefix $(SAMPLES_OBJS_DIR_DBG)/, $(addsuffix .$(OBJSUFFIX), $(notdir $(basename $(SAMPLE_CSRCS)))))
SAMPLE_RCOBJS   = $(addprefix $(SAMPLES_OBJS_DIR_REL)/, $(addsuffix .$(OBJSUFFIX), $(notdir $(basename $(SAMPLE_CSRCS)))))


SAMPLE_EXE_COBJS   = $(addprefix $(SAMPLES_OBJS_DIR)/, $(addsuffix .$(OBJSUFFIX), $(notdir $(basename $(SAMPLE_EXE_CSRCS)))))

SAMPLE_EXE_DCOBJS   = $(addprefix $(SAMPLES_OBJS_DIR)/debug/, $(addsuffix .$(OBJSUFFIX), $(notdir $(basename $(SAMPLE_EXE_CSRCS)))))
SAMPLE_EXE_RCOBJS   = $(addprefix $(SAMPLES_OBJS_DIR)/release/, $(addsuffix .$(OBJSUFFIX), $(notdir $(basename $(SAMPLE_EXE_CSRCS)))))

# Rule to move executables to bin folder
move:
	@mv $(SAMPLES_DIR)/$(SAMPLE).exe         $(SAMPLES_EXES_DIR)/$(SAMPLE)

move_debug:
	@mv $(SAMPLES_DIR)/$(SAMPLE).exe_debug   $(SAMPLES_EXES_DIR)/$(SAMPLE)_debug

move_release:
	@mv $(SAMPLES_DIR)/$(SAMPLE).exe_release $(SAMPLES_EXES_DIR)/$(SAMPLE)_release

endif # ifneq ("$(BUILDOS_LINUX_OR_ANDROID)", "")

############################### LINUX ONLY BUILDS ##############################
## Samples sources Build rules
################################################################################
ifeq ("$(SYSLINK_BUILDOS)", "Linux")

# Compile template for sample source files
define SampleCompile_template
# Rule to build debug
$(SAMPLES_OBJS_DIR_DBG)/$(notdir $(basename $(1)).$(OBJSUFFIX)): $(basename $(1)).c
	@echo Compiling: $(basename $(1)).c
ifeq ("$(GCC_TOOL_WRAPPER)", "1")
	$(Q)$(CC) $(CFLAGS) $(COPTIMIZE) $(COMPILE_FLAGS) -D DEBUG -c -o $$@ $$<
else
	$(Q)$(CC) $(CFLAGS) $(COPTIMIZE) $(COMPILE_FLAGS) -ggdb -D DEBUG -c -o $$@ $$<
endif

# Rule to build release
$(SAMPLES_OBJS_DIR_REL)/$(notdir $(basename $(1)).$(OBJSUFFIX)): $(basename $(1)).c
	@echo Compiling: $(basename $(1)).c
ifeq ("$(GCC_TOOL_WRAPPER)", "1")
	$(Q)$(CC) $(CFLAGS) $(COPTIMIZE) $(COMPILE_FLAGS) -D DEBUG -c -o $$@ $$<
else
	$(Q)$(CC) $(CFLAGS) $(COPTIMIZE) $(COMPILE_FLAGS) -ggdb -D DEBUG -c -o $$@ $$<
endif
endef

# Create rules for compiling C files
$(foreach prog,$(SAMPLE_CSRCS),$(eval $(call SampleCompile_template,$(prog))))
$(foreach prog,$(SAMPLE_EXE_CSRCS),$(eval $(call SampleCompile_template,$(prog))))

# Rule to build libraries
$(SAMPLES_DIR)/$(APP_LIB)_debug:   $(SAMPLE_DCOBJS)
	@echo Building library $@
	$(Q)$(LD) $(SAMPLE_DCOBJS) -r -o $@
$(SAMPLES_DIR)/$(APP_LIB)_release: $(SAMPLE_RCOBJS)
	@echo Building library $@
	$(Q)$(LD) $(SAMPLE_RCOBJS) -r -o $@

# Rule to build executables
$(SAMPLES_DIR)/$(SAMPLE).exe_debug: $(SAMPLE_DCOBJS) $(SAMPLE_EXE_DCOBJS) $(LIB_DIR)/$(SYSLINK_LIB)_debug
	@echo Building $@
ifeq ("$(GCC_TOOL_WRAPPER)", "1")
	$(Q)$(CC) $(LINKFLAGS) -O0 -Wall -o $@ $(SAMPLE_DCOBJS) $(SAMPLE_EXE_DCOBJS)                    \
	$(LIB_DIR)/$(SYSLINK_ARCHIVE)_debug
else
	$(Q)$(CC) $(LINKFLAGS) -lc -O0 -ggdb -Wall -o $@ $(SAMPLE_DCOBJS) $(SAMPLE_EXE_DCOBJS)          \
	$(LIB_DIR)/$(SYSLINK_LIB)_debug  $(APP_DEP_LIB_DEBUG)
endif

$(SAMPLES_DIR)/$(SAMPLE).exe_release: $(SAMPLE_RCOBJS) $(SAMPLE_EXE_RCOBJS) $(LIB_DIR)/$(SYSLINK_LIB)_release
	@echo Building $@
ifeq ("$(GCC_TOOL_WRAPPER)", "1")
	$(Q)$(CC) $(LINKFLAGS) -O3 -Wall -o $@ $(SAMPLE_RCOBJS) $(SAMPLE_EXE_RCOBJS)                \
	$(LIB_DIR)/$(SYSLINK_ARCHIVE)_release
else
	$(Q)$(CC) $(LINKFLAGS) -lc -O3 -Wall -o $@ $(SAMPLE_RCOBJS) $(SAMPLE_EXE_RCOBJS)                \
	$(LIB_DIR)/$(SYSLINK_LIB)_release $(APP_DEP_LIB_RELEASE)
endif

endif # ifeq ("$(SYSLINK_BUILDOS)", "Linux")
