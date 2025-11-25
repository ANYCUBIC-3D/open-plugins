option(ENALBE_TEST "打开Test目录编译" OFF)
option(ENABLE_GTEST "打开GTest目录编译" OFF)

macro(add_subdirTEST subname)
    if(ENALBE_TEST)
        include_directories(
            ${CMAKE_CURRENT_BINARY_DIR}/${subname}
            ${CMAKE_CURRENT_SOURCE_DIR}/${subname}
            )


        file(GLOB_RECURSE HDRS_G "${CMAKE_CURRENT_SOURCE_DIR}/${subname}/*.h*" )
        file(GLOB_RECURSE SRCS_G "${CMAKE_CURRENT_SOURCE_DIR}/${subname}/*_Test.cpp" )
        file(GLOB_RECURSE OtherSRC_G "${CMAKE_CURRENT_SOURCE_DIR}/${subname}/*cpp" )
        foreach(item ${SRCS_G})
            list(REMOVE_ITEM OtherSRC_G ${item})
        endforeach(item)

        foreach(FILE ${SRCS_G})
            cmake_path(GET FILE STEM SUB_NAME)
            add_executable(${SUB_NAME} ${FILE} ${OtherSRC_G})
            target_link_libraries(${SUB_NAME} PRIVATE ${ARGN})
            set_target_properties(${SUB_NAME} PROPERTIES FOLDER "Test")
        endforeach(FILE)
        file(GLOB_RECURSE CMAKE_G "${CMAKE_CURRENT_SOURCE_DIR}/${subname}/*.cmake" )
        foreach(item ${CMAKE_G})
            include(${item})
        endforeach(item)
    endif()
endmacro()


macro(add_subdirGTEST subname)
    if(ENABLE_GTEST)
        find_package(GTest REQUIRED)
        INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})
        message("GTEST_INCLUDE_DIRS=${GTEST_INCLUDE_DIRS} GTEST_BOTH_LIBRARIES=${GTEST_BOTH_LIBRARIES} GTEST_MAIN_LIBRARY=${GTEST_LIBRARY}")
        add_subdirTEST(${subname} ${GTEST_BOTH_LIBRARIES}  ${ARGN})
    endif(ENABLE_GTEST)
endmacro()



function(add_subdirs)
    foreach(sub ${ARGN})
        if (EXISTS ${CMAKE_SOURCE_DIR}/${sub}/CMakeLists.txt)
            add_subdirectory(${CMAKE_SOURCE_DIR}/${sub})  
        else()
            message(WARNING "subdir ${sub} not exists")
        endif()
    endforeach()
endfunction()

function(add_subdirWin32)
    if(CMAKE_HOST_WIN32)
        foreach(sub ${ARGN})
            if (EXISTS ${CMAKE_SOURCE_DIR}/${sub}/CMakeLists.txt)
                add_subdirectory(${CMAKE_SOURCE_DIR}/${sub})  
            endif()
        endforeach()
    endif(CMAKE_HOST_WIN32)
endfunction(add_subdirWin32)

function(add_subdirOSX)
    if(CMAKE_HOST_APPLE)
        foreach(sub ${ARGN})
            if (EXISTS ${CMAKE_SOURCE_DIR}/${sub}/CMakeLists.txt)
                add_subdirectory(${CMAKE_SOURCE_DIR}/${sub})  
            endif()
        endforeach()
    endif(CMAKE_HOST_APPLE)
endfunction(add_subdirOSX)


option(ENABLE_DEMOS "打开Demo目录编译" OFF)
function(add_subdirsDemos)
    if(ENABLE_DEMOS)
        add_subdirs(${ARGN})
    endif()
endfunction()


include(GenerateExportHeader)
macro(add_shared sub_project_name MACRO_NAME)
    add_library(${sub_project_name} SHARED  ${ARGN})
    target_include_directories(${sub_project_name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    target_compile_definitions(${sub_project_name} PRIVATE ${sub_project_name}_EXPORTS)
    if(CMAKE_HOST_APPLE OR CMAKE_HOST_LINUX)
        set_target_properties(${sub_project_name} PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            C_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
        )
        target_compile_options(${sub_project_name} PRIVATE 
            -fvisibility=hidden
            -fvisibility-inlines-hidden
            -Wl,--exclude-libs,ALL
        )
    endif()
    generate_export_header(${sub_project_name} EXPORT_MACRO_NAME ${MACRO_NAME} EXPORT_FILE_NAME ${CMAKE_CURRENT_LIST_DIR}/${sub_project_name}_export.hxx)
endmacro()
macro(add_static sub_project_name MACRO_NAME)
    add_library(${sub_project_name} STATIC  ${ARGN})
    target_include_directories(${sub_project_name} PRIVATE ${CMAKE_BINARY_DIR}/${sub_project_name})
    set_target_properties(${sub_project_name} PROPERTIES DEBUG_POSTFIX "_s${CMAKE_DEBUG_POSTFIX}")
    set_target_properties(${sub_project_name} PROPERTIES RELWITHDEBINFO_POSTFIX "_s")
endmacro()


find_package(wxWidgets 3.1 COMPONENTS core base adv)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
    function(add_static_plugin plugin_name)
        add_static(${plugin_name} ${plugin_name}_EXPORT ${ARGN})
        # 设置分组
        set_target_properties(${plugin_name} PROPERTIES FOLDER "Plugins_Static")
        target_compile_definitions(${plugin_name} PRIVATE prefix=${plugin_name}  wxDEBUG_LEVEL=0 MODULE_NAME="${plugin_name}" ENABLE_STRACE=1)
        target_link_libraries(${plugin_name} PRIVATE plugins_base ${wxWidgets_LIBRARIES})
        list(APPEND PM_DEPS ${plugin_name})
        set(PM_DEPS "${PM_DEPS}" PARENT_SCOPE)
    endfunction()

    function(add_shared_plugin plugin_name)
        add_shared(${plugin_name} ${plugin_name}_EXPORT ${ARGN})
        # 设置分组
        set_target_properties(${plugin_name} PROPERTIES FOLDER "Plugins_Shared")
        target_compile_definitions(${plugin_name} PRIVATE prefix=${plugin_name}  wxDEBUG_LEVEL=0 MODULE_NAME="${plugin_name}" ENABLE_STRACE=1)
        target_link_libraries(${plugin_name} PRIVATE plugins_base ${wxWidgets_LIBRARIES})
        list(APPEND PM_DEPS ${plugin_name})
        set(PM_DEPS "${PM_DEPS}" PARENT_SCOPE)
        # 设置输出文件名
        set_target_properties(${plugin_name} PROPERTIES OUTPUT_NAME plugin_${plugin_name})
    endfunction()
endif()