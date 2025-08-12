macro(module_install suffix)    
    foreach(h  ${ARGN})
        if(EXISTS ${h})
            set(current_file ${h})
        else()
            set(current_file ${${h}})
        endif()
        INSTALL(FILES ${ARGN} DESTINATION ${suffix})
    endforeach() 
endmacro()


macro(file_install target)
    if(TARGET ${target})
        set_target_properties(${target} PROPERTIES PUBLIC_HEADER ${ARGN})
    endif()
endmacro()

macro(SetInstall target)
    # 配置安装
    INSTALL(TARGETS ${target}
            PUBLIC_HEADER DESTINATION include/${target})
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