if NOT EXIST thirdparty\flatbuffers\CMakeLists.txt git submodule init

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

cd %~DP0protocol
..\thirdparty\flatbuffers\build_release\flatc --cpp schema.fbs

cd %~DP0thirdparty\nng
if NOT EXIST build MKDIR build
cd build 
cmake -H.. -B. -G "Visual Studio 15 2017" -A x64 -DCMAKE_INSTALL_PREFIX=%~DP0
devenv nng.sln /Build Debug 
devenv nng.sln /Build Debug /Project INSTALL 
devenv nng.sln /Build Release  
devenv nng.sln /Build Release /Project INSTALL 



