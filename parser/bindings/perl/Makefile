.PHONY: all clean module

MODULE = FormalInput
DATA_DIRECTORY = data
DATA_FILES = $(wildcard $(DATA_DIRECTORY)/*)
PARSER_DIRECTORY = ../../
SOURCES = $(wildcard $(PARSER_DIRECTORY)/*.c)
HEADERS = $(wildcard $(PARSER_DIRECTORY)/*.h)

all: module

clean:
	$(RM) -r $(MODULE)

module: clean
	h2xs -Afn $(MODULE)
	cp $(SOURCES) $(HEADERS) $(MODULE)
	h2xs -Oan $(MODULE)
	cp $(DATA_FILES) $(MODULE)
	cd $(MODULE) && perl Makefile.PL
	$(MAKE) -C $(MODULE) dist
	$(MAKE) -C $(MODULE)
