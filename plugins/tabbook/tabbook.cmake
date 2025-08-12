file(GLOB_RECURSE SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.h*")
add_static_plugin(tabbook ${SRC})
