file(GLOB_RECURSE SRCS_G "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"  "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")

foreach(DIR IN ITEMS utils codec encrypt)
    file(GLOB_RECURSE HXXDRS_G "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/*.hxx")
    module_install(${OS_PREFIX}/include/utility/${DIR} ${HXXDRS_G})
endforeach()

add_static(utility NONE ${SRCS_G})
target_include_directories(utility PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})