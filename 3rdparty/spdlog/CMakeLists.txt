


add_library(spdlog INTERFACE)
target_compile_definitions(spdlog INTERFACE SPDLOG_HEADER_ONLY=1)
target_include_directories(spdlog INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../")