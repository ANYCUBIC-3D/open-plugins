add_library(nlohmann_json INTERFACE)
target_compile_definitions(nlohmann_json INTERFACE SPDLOG_HEADER_ONLY=1)
target_include_directories(nlohmann_json INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../")


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} TYPE INCLUDE FILES_MATCHING PATTERN "*.hpp")
