
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
        INTERFACE_INCLUDE_DIRECTORIES "${@CMAKE_PROJECT_NAME@_ROOTDIR}/include"
    )
    if(CMAKE_HOST_APPLE)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${@CMAKE_PROJECT_NAME@_ROOTDIR}/lib/${CMAKE_OSX_ARCHITECTURES}/lib${suffix}_s.a"
            IMPORTED_LOCATION_DEBUG "${@CMAKE_PROJECT_NAME@_ROOTDIR}/Debug/lib/${CMAKE_OSX_ARCHITECTURES}/lib${suffix}_sd.a"
        )
    elseif(CMAKE_HOST_WIN32)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_IMPLIB_RELEASE "${@CMAKE_PROJECT_NAME@_ROOTDIR}/lib/${suffix}_s.lib"
            IMPORTED_IMPLIB_DEBUG "${@CMAKE_PROJECT_NAME@_ROOTDIR}/Debug/lib/${suffix}_sd.lib"
        )
    elseif(CMAKE_HOST_LINUX)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${@CMAKE_PROJECT_NAME@_ROOTDIR }/lib/${CMAKE_OSX_ARCHITECTURES}/lib${suffix}_s.a"
            IMPORTED_LOCATION_DEBUG "${@CMAKE_PROJECT_NAME@_ROOTDIR }/Debug/lib/${CMAKE_OSX_ARCHITECTURES}/lib${suffix}_sd.a"
        )
    endif()
endfunction()


function(create_shared_target prefix suffix)
    
    set(projectname ${prefix}::${suffix})
    if(TARGET ${projectname})
        return()
    endif()
    message("Creating ${projectname}")
    add_library(${projectname} SHARED IMPORTED)
    set_property(TARGET ${projectname} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_property(TARGET ${projectname} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(${projectname} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${@CMAKE_PROJECT_NAME@_ROOTDIR}/include"
    )
    if(CMAKE_HOST_APPLE)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${@CMAKE_PROJECT_NAME@_ROOTDIR}/lib/${CMAKE_OSX_ARCHITECTURES}/lib${suffix}.dylib"
            IMPORTED_LOCATION_DEBUG "${@CMAKE_PROJECT_NAME@_ROOTDIR}/Debug/lib/${CMAKE_OSX_ARCHITECTURES}/lib${suffix}d.dylib"
        )
    elseif(CMAKE_HOST_WIN32)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_IMPLIB_RELEASE "${@CMAKE_PROJECT_NAME@_ROOTDIR}/lib/${suffix}.lib"
            IMPORTED_LOCATION_RELEASE "${@CMAKE_PROJECT_NAME@_ROOTDIR}/bin/${suffix}.dll"
            IMPORTED_IMPLIB_DEBUG "${@CMAKE_PROJECT_NAME@_ROOTDIR}/Debug/lib/${suffix}d.lib"
            IMPORTED_LOCATION_DEBUG "${@CMAKE_PROJECT_NAME@_ROOTDIR}/Debug/bin/${suffix}d.dll"
            PDB_NAME_DEBUG  "${@CMAKE_PROJECT_NAME@_ROOTDIR}/Debug/pdb/${suffix}d.pdb"
            PDB_NAME_RELEASE "${@CMAKE_PROJECT_NAME@_ROOTDIR}/pdb/${suffix}.pdb"
        )
    elseif(CMAKE_HOST_LINUX)
        set_target_properties(${projectname} PROPERTIES
            IMPORTED_LOCATION_RELEASE "${@CMAKE_PROJECT_NAME@_ROOTDIR }/lib/${CMAKE_OSX_ARCHITECTURES}/lib${suffix}.so"
            IMPORTED_LOCATION_DEBUG "${@CMAKE_PROJECT_NAME@_ROOTDIR }/Debug/lib/${CMAKE_OSX_ARCHITECTURES}/lib${suffix}d.so"
        )
    endif()
endfunction()

function(create_target prefix suffix)
    set(target_name ${prefix}::${suffix})
    if(TARGET ${target_name})
        return()
    endif()
    if(CMAKE_HOST_APPLE)
            set(IMPORTED_LOCATION_RELEASE "${@CMAKE_PROJECT_NAME@_ROOTDIR}/lib/${CMAKE_OSX_ARCHITECTURES}/lib${suffix}.dylib")
            set(IMPORTED_LOCATION_DEBUG "${@CMAKE_PROJECT_NAME@_ROOTDIR}/Debug/lib/${CMAKE_OSX_ARCHITECTURES}/lib${suffix}d.dylib")
    elseif(CMAKE_HOST_WIN32)
            set(IMPORTED_LOCATION_RELEASE "${@CMAKE_PROJECT_NAME@_ROOTDIR}/bin/${suffix}.dll")
            set(IMPORTED_LOCATION_DEBUG "${@CMAKE_PROJECT_NAME@_ROOTDIR}/Debug/bin/${suffix}d.dll")
    elseif(CMAKE_HOST_LINUX)
            set(IMPORTED_LOCATION_RELEASE "${@CMAKE_PROJECT_NAME@_ROOTDIR }/lib/${CMAKE_OSX_ARCHITECTURES}/lib${suffix}.so")
            set(IMPORTED_LOCATION_DEBUG "${@CMAKE_PROJECT_NAME@_ROOTDIR }/Debug/lib/${CMAKE_OSX_ARCHITECTURES}/lib${suffix}d.so")
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(IMPORTED_LOCATION ${IMPORTED_LOCATION_DEBUG})
    else()
        set(IMPORTED_LOCATION ${IMPORTED_LOCATION_RELEASE})
    endif()
    if(EXISTS ${IMPORTED_LOCATION})
        create_shared_target(${prefix} ${suffix})
    else()
        create_static_target(${prefix} ${suffix})
    endif()
endfunction(create_target)
