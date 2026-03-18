file(GLOB_RECURSE SRCS_G "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"  "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
file(GLOB_RECURSE HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")
find_package(Boost CONFIG REQUIRED COMPONENTS nowide)

add_shared(easy_log LOG_API ${SRCS_G})
setTartetBasic(easy_log
    
)
target_compile_definitions(easy_log PUBLIC FMT_HEADER_ONLY=1 SPDLOG_FMT_EXTERNAL=1)
target_include_directories(easy_log PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${Boost_INCLUDE_DIRS})

target_link_libraries(easy_log PUBLIC fmt spdlog iguana Boost::nowide)


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} TYPE INCLUDE FILES_MATCHING PATTERN "*.hxx")