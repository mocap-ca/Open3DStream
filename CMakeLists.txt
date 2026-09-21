cmake_minimum_required(VERSION  3.13)

project(Open3DStream VERSION)

#if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/usr )
#endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_DEBUG_POSTFIX d)

# Version 1.0.6  Subject::parse() clears its own transform list. It is a
#                receiver-side fix, not a wire change: SubjectList::parse()
#                cleared performers and rigidbodies at the call site and never
#                cleared cameras, so a camera's transform list grew by its own
#                size on every full packet.
# Version 1.0.5  Time envelope on SubjectList: clock domain and instance, exact
#                ns, quality, device time, and the timecode's rate and
#                drop-frame flag. Additive - a 1.0.4 receiver reads a 1.0.5
#                packet unchanged, and a 1.0.5 receiver sees time_version 0
#                from a 1.0.4 sender.
# Version 1.0.4  Remove unused subjects, add all matrixes
set(O3DS_VERSION_TAG "1.0.6")

#add_subdirectory(apps/FbxStream)
add_subdirectory(src)     
add_subdirectory(apps/Repeater)
add_subdirectory(apps/SubscribeTest)
add_subdirectory(plugins/mobu)
#add_subdirectory(plugins/maya)

add_subdirectory(apps/Test1)
add_subdirectory(apps/Test2)

