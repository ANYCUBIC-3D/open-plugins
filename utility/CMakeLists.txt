file(GLOB_RECURSE SRCS_G "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"  "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
file(GLOB_RECURSE HXXDRS_G "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")


add_static(utility NONE ${SRCS_G})
target_include_directories(utility PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})