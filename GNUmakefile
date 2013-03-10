.PHONY: all clean test

BINARIES = bin
LIBRARIES = lib
BUILD = build

UNAME = $(strip $(shell uname))

ifeq ($(UNAME),)
	GENERATOR = "MinGW Makefiles"
	IS_MINGW = 1
else
	ifeq ($(findstring MINGW,$(UNAME)),MINGW)
		ifeq ($(findstring msys,$(shell sh --version)),msys)
			GENERATOR = "MSYS Makefiles"
			# Workaround for strange path bug.
			MAKE_COMMAND = /usr/bin/env make
		endif
	else
		GENERATOR = "Unix Makefiles"
	endif
endif

all: | $(BUILD)
	cd $(BUILD) && cmake -G $(GENERATOR) ..
	$(MAKE) --no-print-directory -C $(BUILD)

clean:
ifdef IS_MINGW
	RD /S /Q $(BUILD) $(BINARIES) $(LIBRARIES) 2>NUL || REM
else
	$(RM) -r $(BUILD) $(BINARIES) $(LIBRARIES)
endif

test:
	cd $(BUILD) && ctest

$(BUILD):
	mkdir $@
