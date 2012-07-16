.SECONDEXPANSION:
include config.mk

# GPLUSPLUS=g++
# See [1] for reasoning behind --enable-auto-import option.

r: bin/main.exe
	$<

all: bin/main.exe

bin/main.exe: src/main.cpp src/test.cpp include/DeBruijnBind.hpp | $$(@D)/.f
	$(GPLUSPLUS) -g -Iinclude -I$(BOOST_PATH) -Wl,--enable-auto-import -std=gnu++0x src/main.cpp src/test.cpp -o bin/main.exe -lstdc++

bin/main-vc.exe: src/main.cpp src/test.cpp include/DeBruijnBind.hpp | $$(@D)/.f
	cl -EHsc -Iinclude -I$(BOOST_PATH) src/main.cpp src/test.cpp -Febin/main-vc.exe -Zi -MTd -link -DEBUG

#[1] http://old.nabble.com/link-error---enable-auto-imports-for-details-td20734348.html

# Folder Creation
PRECIOUS: %/.f
%/.f:
	@echo Making $(dir $@)
	@mkdir -p $(dir $@)
	@touch $@
