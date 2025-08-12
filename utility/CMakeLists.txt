file(GLOB_RECURSE SRCS_G "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"  "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")

add_static(utility NONE ${SRCS_G})
target_link_libraries(utility PRIVATE fmt)
target_include_directories(utility PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(utility INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
setTartetBasic(utility
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} TYPE INCLUDE FILES_MATCHING PATTERN "*.hxx")

