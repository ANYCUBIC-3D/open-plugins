
add_library(iguana INTERFACE)
target_compile_definitions(iguana INTERFACE SPDLOG_HEADER_ONLY=1)
target_include_directories(iguana INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../")

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} TYPE INCLUDE FILES_MATCHING PATTERN "*.hpp")


