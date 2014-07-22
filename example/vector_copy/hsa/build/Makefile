#
# level: ./src/build
#

DEPTH = $(shell find_depth)

CORE_DEPTH = $(if $(GONE_TO_BUILD_DIR),../)..

include $(CORE_DEPTH)/make/coredefs

ifneq (,$(strip $(filter w8,$(DRIVER_OS_TYPE))))
SUBDIRS += w8
endif

ifneq (,$(strip $(filter w864a,$(DRIVER_OS_TYPE))))
SUBDIRS += w864a
endif

ifneq (,$(strip $(filter wNow,$(DRIVER_OS_TYPE))))
SUBDIRS += wNow
endif

ifneq (,$(strip $(filter wNow64a,$(DRIVER_OS_TYPE))))
SUBDIRS += wNow64a
endif

ifneq (,$(strip $(filter wNxt,$(DRIVER_OS_TYPE))))
SUBDIRS += wNxt
endif

ifneq (,$(strip $(filter wNxt64a,$(DRIVER_OS_TYPE))))
SUBDIRS += wNxt64a
endif

ifneq (,$(strip $(filter lnx,$(DRIVER_OS_TYPE))))
SUBDIRS += lnx
endif

ifneq (,$(strip $(filter lnx64a,$(DRIVER_OS_TYPE))))
SUBDIRS += lnx64a
endif

include $(CORE_DEPTH)/make/Makefile.build

LOG_DIRS += $(SUBDIRS)

include $(CORE_DEPTH)/make/corerules
