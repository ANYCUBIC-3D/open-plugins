
add_library(fmt INTERFACE)
target_compile_definitions(fmt INTERFACE FMT_HEADER_ONLY=1)
target_include_directories(fmt INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../")