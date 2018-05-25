REM del CMakeCache.txt
set method=%1
set method=%method:"=%
SHIFT
if /i "%method%" == "codeblocks" goto codeblocks
if /i "%method%" == "visualstudio" goto visualstudio 
goto nativemakefiles
:codeblocks
call "../cmake_codeblocks.bat" %1 %2 %3 %4 %5
goto end
:visualstudio
call "../cmake_visual_studio.bat" %1 %2 %3 %4 %5
goto end
:nativemakefiles
call "../cmake_makefile.bat" %1 %2 %3 %4 %5
goto end
:end
