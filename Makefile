export PROJNAME := AHCI
export RESULT := ahci

.PHONY: all

all: $(RESULT)
	@

%: force
	@$(MAKE) -f $(STRIFE_HELPER)/Makefile $@ --no-print-directory
force: ;
