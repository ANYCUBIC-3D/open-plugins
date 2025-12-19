file(GLOB_RECURSE SRCS_G "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"  "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
find_package(Boost CONFIG REQUIRED)
find_package(OpenSSL 1.1 REQUIRED)
find_package(wxWidgets)


add_static(utility NONE ${SRCS_G})
target_compile_definitions(utility PRIVATE BOOST_ALL_NO_LIB)
target_link_libraries(utility PRIVATE fmt ${OpenSSL_LIBRARIES})
target_include_directories(utility PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})
target_include_directories(utility INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
if(NOT wxWidgets_FOUND)
    target_compile_definitions(utility PRIVATE NOT_WXWIDGETS)
endif()
setTartetBasic(utility
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} TYPE INCLUDE FILES_MATCHING PATTERN "*.hxx")

