
if(CMAKE_HOST_WIN32)
    set(OS_PREFIX ${CMAKE_PROJECT_NAME}/win)
elseif(CMAKE_HOST_APPLE)
    set(OS_PREFIX ${CMAKE_PROJECT_NAME}/mac)
elseif(CMAKE_HOST_LINUX)
    set(OS_PREFIX ${CMAKE_PROJECT_NAME}/linux)
endif()


macro(module_install suffix)
    if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug") 
        foreach(h  ${ARGN})
            if(EXISTS ${h})
                set(current_file ${h})
            else()
                set(current_file ${${h}})
            endif()

            INSTALL(FILES ${current_file} DESTINATION ${suffix})
        endforeach() 
    endif()
endmacro()


macro(file_install target)
    if(TARGET ${target})
        set(suffix ${target})
    endif()
    if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug") 
        foreach(h  ${ARGN})
            if(EXISTS ${h})
                INSTALL(FILES ${h} DESTINATION ${OS_PREFIX}/include/${suffix})
            else()
                INSTALL(FILES ${${h}} DESTINATION ${OS_PREFIX}/include/${suffix})
            endif()
        endforeach() 
    endif()
endmacro()



macro(SetInstall target)
    # 配置安装
    
    INSTALL(TARGETS ${target}
        RUNTIME DESTINATION ${OS_PREFIX}/$<$<CONFIG:Debug>:Debug/>bin/${CMAKE_OSX_ARCHITECTURES}
        LIBRARY DESTINATION ${OS_PREFIX}/$<$<CONFIG:Debug>:Debug/>lib/${CMAKE_OSX_ARCHITECTURES}
        ARCHIVE DESTINATION ${OS_PREFIX}/$<$<CONFIG:Debug>:Debug/>lib/${CMAKE_OSX_ARCHITECTURES}
    )
    file_install(${target} ${ARGN})
    if(CMAKE_HOST_WIN32)
        get_target_property(PDB_NAME ${target} PDB_NAME)
        
        if(NOT PDB_NAME)
            file(GLOB_RECURSE PDB_NAME "${OUTPUT_PATH}/${target}*.pdb" )
            # set(PDB_NAME ${OUTPUT_PATH}/${target}$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>.pdb)
        endif()
        INSTALL(FILES ${PDB_NAME} DESTINATION ${OS_PREFIX}/$<$<CONFIG:Debug>:Debug/>pdb)
    endif()
    file_install(${target} ${ARGN})
endmacro()

macro(depend_librarys out)
    foreach(item ${ARGN})
        find_library(libFlag${item} ${item})
        if(libFlag${item})
            message("\t\t${item} add to deps: ${libFlag${item}}")
            list(APPEND ${out} ${libFlag${item}})
        endif()
    endforeach()
endmacro()


macro(setDepends target)
    target_link_libraries(${target} PRIVATE ${ARGN})
endmacro()


function(setTartetBasic target)
    cmake_parse_arguments(P_ARGS "" "" "INSTALL_HEADERS;DEPS;LIBRARIES" ${ARGN})
    # message("target: ${target}, DEPS: ${P_ARGS_DEPS}, LIBRARIES: ${P_ARGS_LIBRARIES}, URL: ${P_ARGS_URL}, INSTALL_HEADERS: ${P_ARGS_INSTALL_HEADERS}")
    SetInstall(${target} ${P_ARGS_INSTALL_HEADERS})
    depend_librarys(libs ${P_ARGS_DEPS})
    target_link_libraries(${target} PRIVATE ${P_ARGS_LIBRARIES} ${libs})
endfunction()



if(MSVC)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    add_definitions(
        -D_CRT_SECURE_NO_WARNINGS
        -D_WINSOCK_DEPRECATED_NO_WARNINGS
        -D_CRT_NONSTDC_NO_WARNINGS
        -DNOMINMAX
        -DUNICODE
        -D_UNICODE
    )
endif(MSVC)