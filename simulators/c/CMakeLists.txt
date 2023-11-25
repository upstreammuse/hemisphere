cmake_minimum_required(VERSION 3.10)
project(Hemisphere VERSION 0.0.1)

add_subdirectory(lib)

add_executable(Hemisphere
	src/bitset.c
	src/data_mem.c
	src/main.c
)
target_include_directories(Hemisphere PUBLIC include)
target_link_libraries(Hemisphere PUBLIC hmath)
install(TARGETS Hemisphere DESTINATION bin)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${Hemisphere_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Hemisphere_VERSION_MINOR}")
include(CPack)
