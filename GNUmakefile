.PHONY: all clean

BINARIES = bin
LIBRARIES = lib
BUILD = build

all: clean | $(BUILD)
	cd $(BUILD) && cmake -G "Unix Makefiles" .. && make

clean:
	$(RM) -r $(BUILD) $(BINARIES) $(LIBRARIES)

$(BUILD):
	mkdir $@
