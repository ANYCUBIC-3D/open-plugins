add_library(spdlog INTERFACE)
target_compile_definitions(spdlog INTERFACE SPDLOG_HEADER_ONLY=1)
target_include_directories(spdlog INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../")


file(GLOB_RECURSE HDRS  "${CMAKE_CURRENT_SOURCE_DIR}/*.h")


foreach(DIR ${HDRS})
    string(REPLACE "${CMAKE_CURRENT_LIST_DIR}" "" sub ${DIR})
    cmake_path(GET sub PARENT_PATH sub)
    module_install(${OS_PREFIX}/include/spdlog${sub} ${DIR})
endforeach()

