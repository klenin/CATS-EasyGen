.PHONY: all clean

BINARIES = bin
LIBRARIES = lib
BUILD = build

ifeq ($(findstring MINGW, $(shell uname -s)), MINGW)
	GENERATOR = "MinGW Makefiles"
else
	GENERATOR = "Unix Makefiles"
endif

all: | $(BUILD)
	cd $(BUILD) && cmake -G $(GENERATOR) ..
	$(MAKE) -C $(BUILD)

clean:
	$(RM) -r $(BUILD) $(BINARIES) $(LIBRARIES)

$(BUILD):
	mkdir $@
