
function(create_static_target prefix suffix)
    
    set(projectname ${prefix}::${suffix})

    if(TARGET ${projectname})
        return()
    endif()

    message("Create create_static_target: ${projectname}")
    
    add_library(${projectname} SHARED IMPORTED)
    set_property(TARGET ${projectname} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_property(TARGET ${projectname} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(${projectname} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ROOT_DIR}include"
    )
    if(CMAKE_HOST_APPLE)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${ROOT_DIR}lib/lib${suffix}_s.a"
            IMPORTED_LOCATION_DEBUG "${ROOT_DIR}lib/lib${suffix}_s${POSTFIX}.a"
        )
    elseif(CMAKE_HOST_WIN32)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_IMPLIB_RELEASE "${ROOT_DIR}lib/${suffix}_s.lib"
            IMPORTED_IMPLIB_DEBUG "${ROOT_DIR}lib/${suffix}_s${POSTFIX}.lib"
        )
    elseif(CMAKE_HOST_LINUX)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${ROOT_DIR}lib/lib${suffix}.a"
            IMPORTED_LOCATION_DEBUG "${ROOT_DIR}lib/lib${suffix}_s${POSTFIX}.a"
        )
    endif()
endfunction()


function(create_shared_target prefix suffix)

    set(projectname ${prefix}::${suffix})
    if(TARGET ${projectname})
        return()
    endif()
    message("Create create_shared_target: ${projectname}")

    add_library(${projectname} SHARED IMPORTED)
    set_property(TARGET ${projectname} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_property(TARGET ${projectname} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(${projectname} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ROOT_DIR}include"
    )
    if (${suffix} STREQUAL "instrument")
        config_instrument(${projectname})
        if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            # 非Debug，没有生成插桩相关库
            return()
        endif()
    endif()
    if(CMAKE_HOST_APPLE)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${ROOT_DIR}lib/lib${suffix}.dylib"
            IMPORTED_LOCATION_DEBUG "${ROOT_DIR}lib/lib${suffix}${POSTFIX}.dylib"
        )
    elseif(CMAKE_HOST_WIN32)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_IMPLIB_RELEASE "${ROOT_DIR}lib/${suffix}.lib"
            IMPORTED_LOCATION_RELEASE "${ROOT_DIR}bin/${suffix}.dll"
            IMPORTED_IMPLIB_DEBUG "${ROOT_DIR}lib/${suffix}${POSTFIX}.lib"

            IMPORTED_LOCATION_DEBUG "${ROOT_DIR}bin/${suffix}${POSTFIX}.dll"
        )
    elseif(CMAKE_HOST_LINUX)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${ROOT_DIR}lib/lib${suffix}.so"
            IMPORTED_LOCATION_DEBUG "${ROOT_DIR}lib/lib${suffix}${POSTFIX}.so"
        )
    endif()
endfunction()

function(create_target prefix suffix)
    set(target_name ${prefix}::${suffix})
    if(TARGET ${target_name})
        return()
    endif()
    set(PREFIX lib)
    if(CMAKE_HOST_WIN32)
        set(file_suffix "dll")
        set(SEARCH_PATH "${ROOT_DIR}bin")
        unset(PREFIX)
    elseif(CMAKE_HOST_APPLE)
        set(file_suffix "dylib")
        set(SEARCH_PATH "${ROOT_DIR}lib")
    elseif(CMAKE_HOST_LINUX)
        set(file_suffix "so")
        set(SEARCH_PATH "${ROOT_DIR}lib")
    endif()

    find_file (${suffix}_IS_SHARED_TYPE NAMES ${PREFIX}${suffix}.${file_suffix} ${PREFIX}${suffix}${POSTFIX}.${file_suffix} ${PREFIX}${suffix}_s.${file_suffix} ${PREFIX}${suffix}_s${POSTFIX}.${file_suffix} PATHS PATH "${SEARCH_PATH}" NO_DEFAULT_PATH)
    if(${suffix}_IS_SHARED_TYPE)
        message("create ${target_name}:${${suffix}_IS_SHARED_TYPE}")
        create_shared_target(${prefix} ${suffix})
        unset(${suffix}_IS_SHARED_TYPE)
    else()
        message("create ${target_name}")
        create_static_target(${prefix} ${suffix})
    endif()
endfunction(create_target)
