
add_library(fmt INTERFACE)
target_compile_definitions(fmt INTERFACE FMT_HEADER_ONLY=1)
target_include_directories(fmt INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../")

file(GLOB_RECURSE HDRS  "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES ${HDRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fmt)