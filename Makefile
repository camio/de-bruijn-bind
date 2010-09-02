BOOST_PATH="C:\\Documents and Settings\\David J. Sankel\\My Documents\\Sankel Software\\working\\boost_1_44_0x86"

GPLUSPLUS=C:\\mingw-get-0.1-mingw32-alpha-3-bin\\bin\\g++
# GPLUSPLUS=g++
# See [1] for reasoning behind --enable-auto-import option.

r: bin/main.exe
	$<

all: bin/main.exe

bin/main.exe: src/main.cpp src/test.cpp include/DeBruijnBind.hpp
	$(GPLUSPLUS) -Iinclude -I$(BOOST_PATH) -Wl,--enable-auto-import -std=gnu++0x src/main.cpp src/test.cpp -o bin/main.exe

bin/main-vc.exe: src/main.cpp src/test.cpp include/DeBruijnBind.hpp
	cl -EHsc -Iinclude -I$(BOOST_PATH) src/main.cpp src/test.cpp -Febin/main-vc.exe

#[1] http://old.nabble.com/link-error---enable-auto-imports-for-details-td20734348.html
