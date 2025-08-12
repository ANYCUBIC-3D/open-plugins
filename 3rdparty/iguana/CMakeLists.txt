
add_library(iguana INTERFACE)
target_compile_definitions(iguana INTERFACE SPDLOG_HEADER_ONLY=1)
target_include_directories(iguana INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../")


file(GLOB_RECURSE HDRS  "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")


foreach(DIR ${HDRS})
    string(REPLACE "${CMAKE_CURRENT_LIST_DIR}" "" sub ${DIR})
    cmake_path(GET sub PARENT_PATH sub)
    module_install(${OS_PREFIX}/include/iguana${sub} ${DIR})
endforeach()

