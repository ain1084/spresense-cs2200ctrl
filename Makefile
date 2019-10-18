-include $(TOPDIR)/Make.defs
-include $(SDKDIR)/Make.defs

CONFIG_CS2200CTRL_PRIORITY ?= SCHED_PRIORITY_DEFAULT
CONFIG_CS2200CTRL_STACKSIZE ?= 2048

APPNAME = cs2200ctrl
PRIORITY = $(CONFIG_CS2200CTRL_PRIORITY)
STACKSIZE = $(CONFIG_CS2200CTRL_STACKSIZE)

MAINSRC = cs2200ctrl_main.c
ASRCS =
CSRCS = $(filter-out $(MAINSRC),$(wildcard *.c) $(wildcard */*.c))
CXXSRCS = $(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard *.cxx) $(wildcard */*.cxx)

CFLAGS += $(foreach inc,$(wildcard ../*/include),-I$(inc))
CXXFLAGS += $(foreach inc,$(wildcard ../*/include),-I$(inc))

CONFIG_CS2200CTRL_PROGNAME ?= cs2200ctrl$(EXEEXT)
PROGNAME = $(CONFIG_CS2200CTRL_PROGNAME)

include $(APPDIR)/Application.mk
