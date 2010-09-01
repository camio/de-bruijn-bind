BOOST_PATH="C:\\Documents and Settings\\David J. Sankel\\My Documents\\Sankel Software\\working\\boost_1_44_0x86"
r: DeBruijnBind.exe
	$<

DeBruijnBind.exe: DeBruijnBind.cpp
	cl -EHsc -I$(BOOST_PATH) $<

