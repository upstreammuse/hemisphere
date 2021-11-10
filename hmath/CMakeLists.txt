add_library(HMath hmath.c)
target_include_directories(HMath INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
install(FILES hmath.h DESTINATION include)
install(TARGETS HMath DESTINATION lib)
