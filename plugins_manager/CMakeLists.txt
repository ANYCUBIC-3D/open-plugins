set(sub_project_name plugins_manager)


find_package(wxWidgets 3.1 REQUIRED COMPONENTS core base adv)
file(GLOB_RECURSE SRCS_G "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"  "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
file(GLOB_RECURSE HXXDRS_G "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")

include(${wxWidgets_USE_FILE})
add_static(${sub_project_name} PM_EXPORT ${SRCS_G} ${HXXDRS_G})
setTartetBasic(${sub_project_name}  INSTALL_HEADERS HXXDRS_G) 

set(AES_PASSWORD "anycubic.com" CACHE STRING "AES加密密码")
set(FILE_SIGN "anycubic.com" CACHE STRING "文件签名")

set(gen_embed_file_header ${CMAKE_CURRENT_BINARY_DIR}/plugins_list.h)
file(WRITE ${gen_embed_file_header} "#define PLUGINS_LIST (")
if(DEFINED PM_DEPS)
    foreach(item ${PM_DEPS})
        file(APPEND ${gen_embed_file_header} "${item},")
    endforeach()
endif()
file(APPEND ${gen_embed_file_header} "NONE)\n")


list(LENGTH PM_DEPS PLUGINS_LIST_SIZE)

target_compile_definitions(${sub_project_name} PRIVATE MODULE_NAME="${sub_project_name}" PLUGINS=1 ENABLE_STRACE=1 wxDEBUG_LEVEL=0 AES_PASSWORD="${AES_PASSWORD}" FILE_SIGN="${FILE_SIGN}" PLUGINS_LIST_SIZE=${PLUGINS_LIST_SIZE})
target_include_directories(${sub_project_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(${sub_project_name} PRIVATE cxx_std_20)
target_link_libraries(${sub_project_name} PRIVATE ACWebView easy_log)
set_target_properties(${sub_project_name} PROPERTIES 
    FOLDER "Plugins"
    INTERFACE_INCLUDE_DIRECTORIES  ${CMAKE_CURRENT_BINARY_DIR}
    )
if(MSVC)
    target_compile_options(${sub_project_name} PRIVATE /utf-8)
endif(MSVC)