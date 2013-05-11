.SECONDEXPANSION:
include config.mk

r: bin/main.exe
	$<

all: bin/main.exe

bin/main.exe: src/main.cpp src/test.cpp include/dbb/dbb.hpp | $$(@D)/.f
	$(GPLUSPLUS) -g -Iinclude -I$(BOOST_PATH) -std=c++11 src/main.cpp src/test.cpp src/test_factorial.cpp src/test_Larry_Evans.cpp src/test_algorithms.cpp src/test_misc.cpp -o bin/main.exe -lstdc++

bin/main-vc.exe: src/main.cpp src/test.cpp include/dbb/dbb.hpp | $$(@D)/.f
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
	
