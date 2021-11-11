cmake_minimum_required(VERSION 3.10)
project(Hemisphere VERSION 0.0.1)

add_subdirectory(hmath)
add_executable(Hemisphere bitset.c data_mem.c main.cpp)
target_link_libraries(Hemisphere PUBLIC HMath)

install(TARGETS Hemisphere DESTINATION bin)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${Hemisphere_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Hemisphere_VERSION_MINOR}")
include(CPack)
