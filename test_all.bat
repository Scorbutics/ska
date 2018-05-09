set PROJECT_PATH=%1
set PROJECT_PATH=%PROJECT_PATH:"=%

copy %PROJECT_PATH%\Debug\ska_core_test.exe %PROJECT_PATH%
copy %PROJECT_PATH%\Debug\ska_ai_test.exe %PROJECT_PATH%
copy %PROJECT_PATH%\Debug\ska_gui_test.exe %PROJECT_PATH%
copy %PROJECT_PATH%\Debug\ska_world_test.exe %PROJECT_PATH%
cd %PROJECT_PATH%

ska_core_test.exe
ska_ai_test.exe
ska_gui_test.exe
ska_world_test.exe

exit /b %errorlevel%