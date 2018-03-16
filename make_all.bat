cd ska_core
call cmake_.bat %target%
mingw32-make
cd ../ska_core_test
call cmake_.bat %target%
mingw32-make
cd ../ska_draw
call cmake_.bat %target%
mingw32-make
cd ../ska_gui
call cmake_.bat %target%
mingw32-make
cd ../ska_gui_test
call cmake_.bat %target%
mingw32-make
cd ../ska_ai
call cmake_.bat %target%
mingw32-make
cd ../ska_particle
call cmake_.bat %target%
mingw32-make
cd ../ska_physic
call cmake_.bat %target%
mingw32-make
cd ../ska_physic_chipmunk
call cmake_.bat %target%
mingw32-make
cd ../ska_physic_chipmunk_test
call cmake_.bat %target%
mingw32-make
cd ../ska_physic_homemade
call cmake_.bat %target%
mingw32-make
cd ../ska_script
call cmake_.bat %target%
mingw32-make
cd ../ska_world
call cmake_.bat %target%
mingw32-make
cd ..