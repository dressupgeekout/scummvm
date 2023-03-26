MODULE := engines/rockett

MODULE_OBJS = \
	clu.o \
	rockett.o \
	console.o \
	metaengine.o \
	presage_archive.o \
	presage_archive_member.o

# This module can be built as a plugin
ifeq ($(ENABLE_ROCKETT), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
