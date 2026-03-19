set(PACKAGES_LIST "$ENV{PACKAGES_LIST}" CACHE STRING "")

include_directories(${CMAKE_CURRENT_BINARY_DIR})


generate_project(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/generate ${PACKAGES_LIST})
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/${CMAKE_PROJECT_NAME}Config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake @ONLY)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/${CMAKE_PROJECT_NAME}Target.cmake ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Target.cmake @ONLY)
set(modules ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake ${CMAKE_SOURCE_DIR}/cmake/${CMAKE_PROJECT_NAME}Instrument.cmake  ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Target.cmake ${CMAKE_SOURCE_DIR}/cmake/generate.cmake ${CMAKE_SOURCE_DIR}/cmake/basic.cmake ${CMAKE_SOURCE_DIR}/cmake/sources.cpp.in)
module_install(share/${PROJECT_NAME} ${modules})

set(HDRS ${CMAKE_CURRENT_SOURCE_DIR}/common.hxx)
if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/resources_export.hxx)
list(APPEND HDRS "${CMAKE_CURRENT_BINARY_DIR}/resources_export.hxx")
endif()
install(FILES ${HDRS} DESTINATION include/resources)

set(DOCS ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
install(FILES ${DOCS} TYPE DOC)
