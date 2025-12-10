
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

option(ENABLE_I18N "Enable i18n support" ON)

if(ENABLE_I18N)
    find_package(Gettext REQUIRED)
    cmake_path(GET GETTEXT_MSGFMT_EXECUTABLE PARENT_PATH CURRENT_PATH)
    find_program(GETTEXT_XGETTEXT_EXECUTABLE xgettext PATHS ${CURRENT_PATH} REQUIRED NO_CACHE NO_DEFAULT_PATH)
endif()


function(i18n_generate_pot target pot destdir)
    if(NOT ENABLE_I18N)
        return()
    endif()
    if(NOT TARGET ${target})
        message(FATAL_ERROR "Target ${target} does not exist")
    endif()
    get_target_property(target_source_files ${target} SOURCES)
    get_target_property(target_name ${target} NAME)
    get_target_property(target_bin_dir ${target} BINARY_DIR)

    set(listfile ${target_bin_dir}/${target_name}.txt)
    file(WRITE ${listfile} "")
    foreach(source_file ${target_source_files})
        file(APPEND ${listfile} "${source_file}\n")
    endforeach()
    # 这里是需要直接生成pot文件
    execute_process(COMMAND ${GETTEXT_XGETTEXT_EXECUTABLE} --package-name=${target_name} --from-code=UTF-8 --keyword=L --keyword=_ --keyword=_L --keyword=_u8L --keyword=L_CONTEXT:1,2c --keyword=_L_PLURAL:1,2 --add-comments=TRN -n --debug --boost -f ${listfile} -o ${pot}
        WORKING_DIRECTORY ${target_bin_dir} ERROR_QUIET
        COMMAND_ECHO STDOUT ECHO_ERROR_VARIABLE ECHO_OUTPUT_VARIABLE)
    if(NOT EXISTS ${pot})
        message(WARNING "target ${target_name} pot file does not exist")
        return()
    endif()
    # 合并po信息
    if(NOT destdir MATCHES "/$")
        set(destdir "${destdir}/")
    endif()
    foreach(lan ${ARGN})
        if(NOT EXISTS ${destdir}${lan})
            file(MAKE_DIRECTORY ${destdir}${lan})
        endif()
        set(_poFile ${destdir}${lan}/${target_name}.po)
        if(EXISTS ${_poFile})
            execute_process(COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} -q -U -N ${_poFile} ${pot}
                WORKING_DIRECTORY ${destdir}${lan} ERROR_QUIET
                COMMAND_ECHO STDOUT ECHO_ERROR_VARIABLE ECHO_OUTPUT_VARIABLE)
        else()
            # Read the generated .po file
            file(READ ${pot} POT_CONTENT)
            # Replace CHARSET with UTF-8
            string(REPLACE "Content-Type: text/plain; charset=CHARSET" "Content-Type: text/plain; charset=UTF-8" POT_CONTENT_UTF8 "${POT_CONTENT}")
            # Write the updated content back to the file
            file(WRITE ${_poFile} "${POT_CONTENT_UTF8}")
        endif()
    endforeach()
endfunction()

function(i18n_generate_mo search_dir)
    if(NOT ENABLE_I18N)
        return()
    endif()
    file(GLOB_RECURSE po_files ${search_dir}/*.po)
    foreach(po_file ${po_files})
        cmake_path(GET po_file PARENT_PATH CURRENT_PATH)
        cmake_path(GET po_file STEM po_filename)
        cmake_path(GET CURRENT_PATH STEM LANGUAGE_NAME)
        set(mo_file ${search_dir}${LANGUAGE_NAME}/${po_filename}.mo)
        
        execute_process(COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --check-format -o ${mo_file} ${po_file}
            WORKING_DIRECTORY ${CURRENT_PATH} ERROR_QUIET
            COMMAND_ECHO STDOUT ECHO_ERROR_VARIABLE ECHO_OUTPUT_VARIABLE)
    endforeach()
endfunction()

function(generate_mo_source_file SRC PACKAGE TEMPLATE)
    file(MD5 ${PACKAGE} PACKAGE_MD5)
    file(SIZE ${PACKAGE} PACKAGE_SIZE)
     # 判断文件大小是否为 0，若为 0 则终止 CMake 配置
     if (PACKAGE_SIZE EQUAL 0)
        message(FATAL_ERROR "file ${PACKAGE} is empty")
    endif()
    # read hex data from file
    file(READ ${PACKAGE} PACKAGE_DATA HEX)
    # convert hex data for C compatibility
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," PACKAGE_DATA "${PACKAGE_DATA}")

    configure_file(${TEMPLATE} ${SRC} @ONLY)
endfunction()


macro(i18n_generate_embed target search_dir)
    if(NOT ENABLE_I18N)
        return()
    endif()
    if(NOT TARGET ${target})
        message(FATAL_ERROR "Target ${target} does not exist")
    endif()
    get_target_property(target_bin_dir ${target} BINARY_DIR)
    set(template ${target_bin_dir}/mo_template.cpp.in)
    set(header ${target_bin_dir}/${target}_languages.hpp)
    file(WRITE ${template}
        "#include <stdint.h>\n"
        "#include <tuple>\n"
        "using result_type = std::tuple<uint32_t, const uint8_t *, const char *>;\n"
        "// clang-format off\n"
        "static const char* PACKAGE_MD5 = \"@PACKAGE_MD5@\";\n"
        "static constexpr uint32_t PACKAGE_SIZE = @PACKAGE_SIZE@;\n"
        "static const uint8_t PACKAGE_DATA[] = {@PACKAGE_DATA@};\n"
        "result_type @LANG@() {\n"
        "return std::make_tuple(PACKAGE_SIZE, PACKAGE_DATA, PACKAGE_MD5);\n"
        "}\n"
        "// clang-format on\n"
        "\n"
        )
    file(WRITE ${header}
        "#include <stdint.h>\n"
        "#include <tuple>\n"
        "using result_type = std::tuple<uint32_t, const uint8_t *, const char *>;\n"
        "#define LANGUAGE_LIST ("
        )
    file(GLOB_RECURSE mo_files ${search_dir}/*.mo)
    
    foreach(mo_file ${mo_files})
        cmake_path(GET mo_file STEM mo_filename)
        cmake_path(GET mo_file PARENT_PATH CURRENT_PATH)
        cmake_path(GET CURRENT_PATH STEM LANG)
        file(APPEND ${header}
            "${LANG},"
            )
        set(embed_file ${target_bin_dir}/${mo_filename}_${LANG}.cpp)
        list(APPEND target_source_files ${embed_file})
        generate_mo_source_file(${embed_file} ${mo_file} ${template})
    endforeach()
    file(APPEND ${header}
        "padding)\n"
        )
    set_source_files_properties(${target_source_files} PROPERTIES GENERATED TRUE)
    target_sources(${target} PRIVATE ${target_source_files})
    target_include_directories(${target} PRIVATE ${target_bin_dir})
endmacro()

