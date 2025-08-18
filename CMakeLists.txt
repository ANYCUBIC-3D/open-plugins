cmake_minimum_required(VERSION 3.20.1)
project(OpenPlugins VERSION 1 LANGUAGES  CXX C)
set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_OSX_DEPLOYMENT_TARGET 11.5)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif(MSVC)

#这个是为了支持Visual Studio的工程分组功能
set_property(GLOBAL PROPERTY USE_FOLDERS  On)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "default set build type Debug" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# CMAKE_GENERATOR
string(FIND ${CMAKE_GENERATOR} "Visual Studio" VS_GENERATOR)
if(VS_GENERATOR EQUAL -1)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
    # Windows DLLs are "runtime" for CMake. Output them to "bin" like the Visual Studio projects do.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
    set(OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
    # Windows DLLs are "runtime" for CMake. Output them to "bin" like the Visual Studio projects do.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
    set(OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${CMAKE_BUILD_TYPE})
endif()


# Append our module directory to CMake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
link_directories(
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} 
    ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    )




include(config OPTIONAL)

foreach(dir ${CMAKE_PREFIX_PATH})
    include_directories(${dir} ${dir}/include)
    if (EXISTS ${dir}/lib)
        link_directories(${dir}/lib)
    endif()
    link_directories(${dir}/$<$<CONFIG:Debug>:Debug/>lib)
endforeach()

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

include(config OPTIONAL)
include(basic)
include(subdir)
include(build)
include(generate)

set(CMAKE_DEBUG_POSTFIX d)
generate_build_header(${CMAKE_BINARY_DIR}/build_info.h)

find_package(wxWidgets 3.1 REQUIRED COMPONENTS core base)
add_subdirs(3rdparty easy_log utility WebView resources plugins_base plugins_sdk plugins plugins_manager)

