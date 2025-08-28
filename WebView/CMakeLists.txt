set(sub_project_name ACWebView)



include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(wxWidgets 3.1 REQUIRED COMPONENTS core base)
set(USE_SANDBOX OFF)

set(USE_SANDBOX OFF)
include(${wxWidgets_USE_FILE})
# Add sources from a ..._SRC variable and headers from a ..._HDR
macro(wx_append_sources src_var source_base_name)
    if(NOT DEFINED ${src_var})
        set(${src_var} "")
    endif()
    if(DEFINED ${source_base_name}_SRC)
        wx_list_add_prefix(${src_var} "${CMAKE_CURRENT_SOURCE_DIR}/" ${${source_base_name}_SRC})
    endif()
    if(DEFINED ${source_base_name}_HDR)
        wx_add_headers(${src_var} ${${source_base_name}_HDR})
    endif()
endmacro()

# Add prefix to list of items
# wx_list_add_prefix(<out_var> <pref> <items...>)
macro(wx_list_add_prefix out_var prefix)
    foreach(item ${ARGN})
        list(APPEND ${out_var} ${prefix}${item})
    endforeach()
endmacro()


# This function adds a list of headers to a variable while prepending
# include/ to the path
macro(wx_add_headers src_var)
    set(headers)
    foreach(header ${ARGN})
        list(APPEND headers ${CMAKE_CURRENT_SOURCE_DIR}/include/${header})
    endforeach()
    list(APPEND ${src_var} ${headers})
endmacro()


set(WEBVIEW_MSW_SRC
    src/msw/webview_edge.cpp
)

set(WEBVIEW_CMN_SRC
    src/common/webview.cpp
    src/common/webviewarchivehandler.cpp
    src/common/webviewfshandler.cpp
    src/common/init.cpp
)

set(WEBVIEW_MSW_HDR
    msw/webview_edge.h
)

set(WEBVIEW_CMN_HDR
    webview.h
    webviewarchivehandler.h
    webviewfshandler.h
    init.h
)

set(WEBVIEW_OSX_SHARED_HDR
    osx/webviewhistoryitem_webkit.h
    osx/webview_webkit.h
)

set(WEBVIEW_OSX_SHARED_SRC
    src/osx/webview_webkit.mm
)

set(WEBVIEW_GTK_HDR
    gtk/webviewhistoryitem_webkit.h
    gtk/webview_webkit.h
)

set(WEBVIEW_GTK_SRC
    src/gtk/webview_webkit.cpp
)

set(WEBVIEW2_GTK_SRC
    src/gtk/webview_webkit2.cpp
)

set(WEBVIEW_WEBKIT2_EXTENSION_SRC
    src/gtk/webview_webkit2_extension.cpp
)

set(WEBVIEWCHROMIUM_HDR
    webview.h
    webview_chromium.h
    webview_chromium_impl.h
)
set(WEBVIEWCHROMIUM_SRC
    src/common/webview_chromium.cpp
)
set(WEBVIEWCHROMIUM_OSX_SHARED_SRC
    src/osx/webview_chromium.mm
)

wx_append_sources(WEBVIEW_FILES WEBVIEW_CMN)


if(CMAKE_HOST_WIN32)
    set(USE_WEBVIEW_EDGE 1)
    wx_append_sources(WEBVIEW_FILES WEBVIEW_MSW)
elseif(CMAKE_HOST_LINUX)
    set(USE_WEBVIEW_EDGE 0)
    set(USE_WEBVIEW_WEBKIT 1)
    set(USE_WEBVIEW_WEBKIT2 1)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_check_modules(WEBKIT2GTK REQUIRED webkit2gtk-4.1)
    if(USE_WEBVIEW_WEBKIT2)
        wx_append_sources(WEBVIEW_FILES WEBVIEW2_GTK)
    elseif(USE_WEBVIEW_WEBKIT)
        wx_append_sources(WEBVIEW_FILES WEBVIEW_GTK)
    endif()
elseif(CMAKE_HOST_APPLE)
    set(USE_WEBVIEW_EDGE 0)
    set(USE_WEBVIEW_WEBKIT 1)
    set(USE_WEBVIEW_CHROMIUM 0)
    wx_append_sources(WEBVIEW_FILES WEBVIEW_OSX_SHARED)
endif()
set(DEVTOOL_ENALBED "ACN_DEVTOOL_ENABLED" CACHE STRING "Enable devtool")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/setup.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/setup.h)


if(wxWidgets_USE_STATIC)
    set(LIB_TYPE STATIC)
else()
    set(LIB_TYPE SHARED)
endif()

add_library(${sub_project_name} ${LIB_TYPE} ${WEBVIEW_FILES})
include(GenerateExportHeader)

generate_export_header(${sub_project_name} EXPORT_MACRO_NAME ACWEBVIEW_API EXPORT_FILE_NAME ${CMAKE_CURRENT_LIST_DIR}/include/${sub_project_name}_export.h)

if(USE_WEBVIEW_EDGE)
    set(WEBVIEW2_VERSION "1.0.3179.45")
    set(WEBVIEW2_URL "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/${WEBVIEW2_VERSION}")
    set(WEBVIEW2_SHA256 "70bd381c5d67f97a6a80294001cde125ab5b24b6d36098ad901d8e15efd752d0")
    set(WEBVIEW2_PACKAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/WebView2")

    
    find_path(WEBVIEW2_FIND_PACKAGE_DIR
            NAMES build/native/include/WebView2.h
            NO_CACHE NO_DEFAULT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_INSTALL_PREFIX
            PATHS 
            ${WEBVIEW2_PACKAGE_DIR}
        )
    if (NOT WEBVIEW2_FIND_PACKAGE_DIR)
        message("=======> WebView2 download start")
        message("=======> WEBVIEW2_VERSION ${WEBVIEW2_VERSION}")
        message("=======> WEBVIEW2_URL ${WEBVIEW2_URL}")
        message("=======> WEBVIEW2_SHA256 ${WEBVIEW2_SHA256}")
        message("=======> WEBVIEW2_PACKAGE_DIR ${WEBVIEW2_PACKAGE_DIR}")
        file(DOWNLOAD
                    ${WEBVIEW2_URL}
                    ${CMAKE_CURRENT_BINARY_DIR}/webview2.nuget
                    EXPECTED_HASH SHA256=${WEBVIEW2_SHA256}
                    SHOW_PROGRESS)
        file(MAKE_DIRECTORY ${WEBVIEW2_PACKAGE_DIR})
        execute_process(COMMAND
                    "${CMAKE_COMMAND}" -E tar xfv "${CMAKE_CURRENT_BINARY_DIR}/webview2.nuget"
                    WORKING_DIRECTORY "${WEBVIEW2_PACKAGE_DIR}"
                )
        message("=======> WebView2 download success")
    else()
        message("=======> WEBVIEW2_PACKAGE_DIR ${WEBVIEW2_PACKAGE_DIR}")
        message("=======> WebView2 find success")
        set(WEBVIEW2_PACKAGE_DIR ${WEBVIEW2_FIND_PACKAGE_DIR})
    endif()
    cmake_path(APPEND WEBVIEW2_PACKAGE_DIR build/native/include OUTPUT_VARIABLE WEBVIEW2_INCLUDE_DIR)
    cmake_path(APPEND WEBVIEW2_PACKAGE_DIR build/native/x64/WebView2LoaderStatic.lib OUTPUT_VARIABLE WEBVIEW2_STATIC_LIBRARY)
    message("=======> WEBVIEW2_INCLUDE_DIR ${WEBVIEW2_INCLUDE_DIR}")
    message("=======> WEBVIEW2_STATIC_LIBRARY ${WEBVIEW2_STATIC_LIBRARY}")
    set_target_properties(${sub_project_name} PROPERTIES 
        INTERFACE_LINK_LIBRARIES "${WEBVIEW2_STATIC_LIBRARY}"
    )
    target_link_libraries(${sub_project_name} PRIVATE "${WEBVIEW2_STATIC_LIBRARY}")

    target_include_directories(${sub_project_name} PUBLIC "${WEBVIEW2_INCLUDE_DIR}")
endif()


set_target_properties(${sub_project_name} PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES  "${CMAKE_CURRENT_BINARY_DIR};${CMAKE_CURRENT_SOURCE_DIR}/include;${WEBVIEW2_INCLUDE_DIR};${WEBKIT2GTK_INCLUDE_DIRS};${GTK3_INCLUDE_DIRS}"
)
target_link_libraries(${sub_project_name} PRIVATE ${wxWidgets_LIBRARIES} )
target_include_directories(${sub_project_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${WEBKIT2GTK_INCLUDE_DIRS} ${GTK3_INCLUDE_DIRS})
target_include_directories(${sub_project_name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${sub_project_name} PRIVATE  "${wxWidgets_INCLUDE_DIRS};${CMAKE_BINARY_DIR}/${sub_project_name};${DIR}")
target_compile_definitions(${sub_project_name} PUBLIC USE_WEBVIEW=1 wxDEBUG_LEVEL=0 WXMAKINGDLL_WEBVIEW=1)
target_compile_definitions(${sub_project_name} PRIVATE DEVTOOL_ENALBED="${DEVTOOL_ENALBED}" ACWebView_EXPORTS=1)


if(CMAKE_HOST_APPLE)
    target_compile_definitions(${sub_project_name} PRIVATE WCHAR_T_IS_16_BIT __STDC_CONSTANT_MACROS __STDC_FORMAT_MACROS)
    if(NOT wxWidgets_USE_STATIC)
        target_link_libraries(${sub_project_name} PRIVATE "-framework Cocoa" "-framework WebKit")  
    endif()
    target_link_libraries(${sub_project_name} INTERFACE  "-framework WebKit")
elseif(CMAKE_HOST_WIN32)
    # target_link_libraries(${sub_project_name} INTERFACE  WebView2)
elseif(CMAKE_HOST_LINUX)
    target_link_libraries(${sub_project_name} INTERFACE  ${WEBKIT2GTK_LIBRARIES} ${GTK3_LIBRARIES})
endif()

# 过滤WEBVIEW_FILES中头文件，并根据目录执行安装
file(GLOB_RECURSE WEBVIEW_FILES "${WEBVIEW_FILES}" "*.h")

SetInstall(${sub_project_name})
foreach(WEBVIEW_FILE ${WEBVIEW_FILES})
    cmake_path(GET WEBVIEW_FILE PARENT_PATH WEBVIEW_FILE_PATH)
    cmake_path(RELATIVE_PATH WEBVIEW_FILE_PATH
        BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include"
        OUTPUT_VARIABLE REL_PATH
    )
    if(REL_PATH STREQUAL ".")
        set(REL_PATH "")
    else()
        cmake_path(NORMAL_PATH REL_PATH) 
    endif()
    INSTALL(FILES ${WEBVIEW_FILE} DESTINATION "include/webview/${REL_PATH}")
endforeach()
