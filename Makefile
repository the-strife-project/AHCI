export PROJNAME := AHCI
export RESULT := ahci

.PHONY: all

all: $(RESULT)
	@

%: force
	@$(MAKE) -f ../helper/Makefile $@ --no-print-directory
force: ;
