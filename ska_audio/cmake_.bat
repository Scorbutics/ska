del CMakeCache.txt
if /i "%1" == "codeblocks" (call "../cmake_codeblocks.bat" ) else (call "../cmake_makefile.bat" )
