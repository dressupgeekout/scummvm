MODULE := engines/rockett

MODULE_OBJS = \
	choice.o \
	clu.o \
	rockett.o \
	console.o \
	metaengine.o \
	presage_archive.o \
	presage_archive_member.o \
	tableau.o \
	xpk.o \
	xpk_decoder.o

# This module can be built as a plugin
ifeq ($(ENABLE_ROCKETT), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
