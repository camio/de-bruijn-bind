.SECONDEXPANSION:
include config.mk

r: bin/main.exe
	$<

all: bin/main.exe

bin/main.exe: src/main.cpp src/test.cpp include/DeBruijnBind.hpp | $$(@D)/.f
	$(GPLUSPLUS) -g -Iinclude -I$(BOOST_PATH) -std=gnu++0x src/main.cpp src/test.cpp -o bin/main.exe -lstdc++

bin/main-vc.exe: src/main.cpp src/test.cpp include/DeBruijnBind.hpp | $$(@D)/.f
	cl -EHsc -Iinclude -I$(BOOST_PATH) src/main.cpp src/test.cpp -Febin/main-vc.exe -Zi -MTd -link -DEBUG

# Folder Creation
PRECIOUS: %/.f
%/.f:
	@echo Making $(dir $@)
	@mkdir -p $(dir $@)
	@touch $@

.PHONY: clean
clean:
	$(RM) -r bin
	
