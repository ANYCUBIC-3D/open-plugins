set(sub_project_name plugins_base)

find_package(Boost CONFIG REQUIRED)


file(GLOB_RECURSE SRCS_G "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"  "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
file(GLOB_RECURSE HXXDRS_G "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")






add_shared(${sub_project_name} PLGINS_EXPORT ${SRCS_G} ${HXXDRS_G})
setTartetBasic(${sub_project_name}) 
target_compile_definitions(${sub_project_name} PRIVATE MODULE_NAME="${sub_project_name}" DENABLE_STRACE=1)
target_include_directories(${sub_project_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${Boost_INCLUDE_DIRS})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} TYPE INCLUDE FILES_MATCHING PATTERN "*.hxx")


set_target_properties(${sub_project_name} PROPERTIES 
    FOLDER "Plugins"
    INTERFACE_INCLUDE_DIRECTORIES  ${CMAKE_CURRENT_BINARY_DIR}
    )
if(MSVC)
    target_compile_options(${sub_project_name} PRIVATE /utf-8)
endif(MSVC)