cd ska_core
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_core_test
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_draw
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_gui
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_gui_test
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_ai
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_ai_test
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_audio
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_particle
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_particle_benchmarker
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_physic_chipmunk
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_physic_chipmunk_test
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_physic_homemade
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_script
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_world
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_physic_simulation_chipmunk
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ../ska_world_test
call "../cmake_.bat" %*
mingw32-make
if %errorlevel% neq 0 exit /b %errorlevel%
cd ..