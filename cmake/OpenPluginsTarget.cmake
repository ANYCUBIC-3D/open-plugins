
function(create_static_target prefix suffix)
    
    set(projectname ${prefix}::${suffix})
    if(TARGET ${projectname})
        return()
    endif()
    message("Creating ${projectname}")
    add_library(${projectname} SHARED IMPORTED)
    set_property(TARGET ${projectname} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_property(TARGET ${projectname} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(${projectname} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ROOT_DIR}/include"
    )
    if(CMAKE_HOST_APPLE)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${ROOT_DIR}/lib/lib${suffix}_s.a"
            IMPORTED_LOCATION_DEBUG "${ROOT_DIR}/lib/lib${suffix}_s${POSTFIX}.a"
        )
    elseif(CMAKE_HOST_WIN32)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_IMPLIB_RELEASE "${ROOT_DIR}/lib/${suffix}_s.lib"
            IMPORTED_IMPLIB_DEBUG "${ROOT_DIR}/lib/${suffix}_s${POSTFIX}.lib"
        )
    elseif(CMAKE_HOST_LINUX)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${ROOT_DIR}/lib/lib${suffix}_s.a"
            IMPORTED_LOCATION_DEBUG "${ROOT_DIR}/lib/lib${suffix}_s${POSTFIX}.a"
        )
    endif()
endfunction()


function(create_shared_target prefix suffix)
    
    set(projectname ${prefix}::${suffix})

 
    add_library(${projectname} SHARED IMPORTED)
    set_property(TARGET ${projectname} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_property(TARGET ${projectname} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(${projectname} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ROOT_DIR}/include"
    )
    if(CMAKE_HOST_APPLE)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${ROOT_DIR}/lib/lib${suffix}.dylib"
            IMPORTED_LOCATION_DEBUG "${ROOT_DIR}/lib/lib${suffix}${POSTFIX}.dylib"
        )
    elseif(CMAKE_HOST_WIN32)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_IMPLIB_RELEASE "${ROOT_DIR}/lib/${suffix}.lib"
            IMPORTED_LOCATION_RELEASE "${ROOT_DIR}/bin/${suffix}.dll"
            IMPORTED_IMPLIB_DEBUG "${ROOT_DIR}/lib/${suffix}${POSTFIX}.lib"
            IMPORTED_LOCATION_DEBUG "${ROOT_DIR}/bin/${suffix}${POSTFIX}.dll"
            PDB_NAME_DEBUG  "${ROOT_DIR}/pdb/${suffix}${POSTFIX}.pdb"
            PDB_NAME_RELEASE "${ROOT_DIR}/pdb/${suffix}.pdb"
        )
    elseif(CMAKE_HOST_LINUX)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${ROOT_DIR}/lib/lib${suffix}.so"
            IMPORTED_LOCATION_DEBUG "${ROOT_DIR}/lib/lib${suffix}${POSTFIX}.so"
        )
    endif()
endfunction()

function(create_target prefix suffix)
    set(target_name ${prefix}::${suffix})
    if(TARGET ${target_name})
        return()
    endif()

    find_library(${suffix}_IMPORTED_LOCATION NAMES ${suffix} ${suffix}${POSTFIX} ${suffix}_s ${suffix}_s${POSTFIX} PATHS PATH "${ROOT_DIR}lib" NO_DEFAULT_PATH)
    if(NOT ${suffix}_IMPORTED_LOCATION)
        message(FATAL_ERROR "Cannot find library ${suffix} or ${suffix}${POSTFIX}")
    endif()
    
    if(${${suffix}_IMPORTED_LOCATION} MATCHES ".*\\.(a|lib)$")
        create_static_target(${prefix} ${suffix})
    else()
        create_shared_target(${prefix} ${suffix})
    endif()
endfunction(create_target)