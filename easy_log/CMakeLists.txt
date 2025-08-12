file(GLOB_RECURSE SRCS_G "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"  "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
file(GLOB_RECURSE HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")
add_shared(easy_log LOG_API ${SRCS_G})
setTartetBasic(easy_log
    LIBRARIES fmt spdlog iguana
)
target_compile_definitions(easy_log PUBLIC FMT_HEADER_ONLY=1 SPDLOG_FMT_EXTERNAL=1)
target_include_directories(easy_log PUBLIC ${CMAKE_CURRENT_BINARY_DIR})


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} TYPE INCLUDE FILES_MATCHING PATTERN "*.hxx")