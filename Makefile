.SECONDEXPANSION:
include config.mk

SOURCES = $(wildcard src/*.cpp)
HEADERS = $(wildcard include/dbb/*.hpp include/dbb/*/*.hpp)

r: bin/main.exe
	$<

all: bin/main.exe

bin/main.exe: $(SOURCES) $(HEADERS) | $$(@D)/.f
	$(GPLUSPLUS) -g -Iinclude -I$(BOOST_PATH) -std=c++11 $(SOURCES) -o $@ -lstdc++

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
	
