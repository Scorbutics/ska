set target=%1

cd ska_core
call cmake_.bat %target%
cd ../ska_core_test
call cmake_.bat %target%
cd ../ska_draw
call cmake_.bat %target%
cd ../ska_gui
call cmake_.bat %target%
cd ../ska_gui_test
call cmake_.bat %target%
cd ../ska_ai
call cmake_.bat %target%
cd ../ska_particle
call cmake_.bat %target%
cd ../ska_physic
call cmake_.bat %target%
cd ../ska_physic_chipmunk
call cmake_.bat %target%
cd ../ska_physic_chipmunk_test
call cmake_.bat %target%
cd ../ska_physic_homemade
call cmake_.bat %target%
cd ../ska_physic_simulation
call cmake_.bat %target%
cd ../ska_script
call cmake_.bat %target%
cd ../ska_world
call cmake_.bat %target%
cd ..