file(GLOB_RECURSE SRC_G "${CMAKE_CURRENT_LIST_DIR}/*.h*" "${CMAKE_CURRENT_LIST_DIR}/*.c" "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_static_plugin(app ${SRC_G})

