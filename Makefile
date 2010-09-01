BOOST_PATH="C:\\Documents and Settings\\David J. Sankel\\My Documents\\Sankel Software\\working\\boost_1_44_0x86"

GPLUSPLUS=C:\\mingw-get-0.1-mingw32-alpha-3-bin\\bin\\g++
# GPLUSPLUS=g++

r: DeBruijnBind2.exe
	$<

DeBruijnBind2.exe: DeBruijnBind2.cpp
	cl -EHsc -I$(BOOST_PATH) $<

test:
	$(GPLUSPLUS) -I$(BOOST_PATH) -Wl,--enable-auto-import -std=gnu++0x DeBruijnBind2.cpp -o DeBruijnBind2.exe
# 	$(GPLUSPLUS) --version
