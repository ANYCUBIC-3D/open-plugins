
include(${CMAKE_CURRENT_LIST_DIR}/basic.cmake)



include(GenerateExportHeader)

macro(add_rc_shared sub_project_name MACRO_NAME)
    add_library(${sub_project_name} SHARED ${ARGN})
    target_include_directories(${sub_project_name} PRIVATE 
        ${CMAKE_BINARY_DIR}/${sub_project_name}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_compile_definitions(${sub_project_name} PRIVATE ${sub_project_name}_EXPORTS)
    
    # 统一可见性设置
    set_target_properties(${sub_project_name} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
    target_compile_options(${sub_project_name} PRIVATE -fvisibility=hidden)
    generate_export_header(${sub_project_name} 
        EXPORT_MACRO_NAME ${MACRO_NAME}
        EXPORT_FILE_NAME ${sub_project_name}_export.hxx
    )
endmacro()


function(generate_source_file SRC PACKAGE_NAME PACKAGE)
    set(TEMPLATE ${CMAKE_CURRENT_LIST_DIR}/sources.cpp.in)
    if(NOT EXISTS ${TEMPLATE})
        set(TEMPLATE ${CMAKE_MODULE_PATH}/sources.cpp.in)
    endif()
    
    file(READ ${PACKAGE} filedata HEX)
    string(REGEX REPLACE "([0-9a-f]{2})" "0x\\1," PACKAGE_DATA ${filedata})
    configure_file(${TEMPLATE} ${SRC} @ONLY)
endfunction()


function(generate_project include_dir work_dir)
    foreach(PACKAGE IN ITEMS ${ARGN})
        get_filename_component(PACKAGE_NAME ${PACKAGE} NAME_WE)
        string(MAKE_C_IDENTIFIER ${PACKAGE_NAME} PACKAGE_NAME)
        
        generate_source_file("${work_dir}/${PACKAGE_NAME}.cpp" ${PACKAGE_NAME} ${PACKAGE})
        add_rc_shared(${PACKAGE_NAME} ${PACKAGE_NAME}_EXPORT "${work_dir}/${PACKAGE_NAME}.cpp")
        
        target_include_directories(${PACKAGE_NAME} PRIVATE ${include_dir})
        set_property(TARGET ${PACKAGE_NAME} PROPERTY FOLDER "resources")
        SetInstall(${PACKAGE_NAME})
    endforeach()
    
    if(ARGN)
        configure_file(${CMAKE_CURRENT_LIST_DIR}/GenerateTarget.cmake.in
            "${CMAKE_CURRENT_BINARY_DIR}/GenerateTarget.cmake" @ONLY)
        module_install(share/${PROJECT_NAME} "${CMAKE_CURRENT_BINARY_DIR}/GenerateTarget.cmake")
    endif()
endfunction()