git submodule update
cd thirdparty\flatbuffers
if NOT EXIST build_release MKDIR build_release
cd build_release 
cmake -H.. -B. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
nmake
cd ..

if NOT EXIST build_debug MKDIR build_debug
cd build_debug 
cmake -H.. -B. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug
nmake

cd ..\..\..\protocol
..\thirdparty\flatbuffers\build_release\flatc --cpp schema.fbs




