add_library(hmath hmath.c)
target_include_directories(hmath INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
install(FILES hmath.h DESTINATION include)
install(TARGETS hmath DESTINATION lib)
