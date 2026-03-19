file(GLOB SRCS_G "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"  "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
if(CMAKE_HOST_WIN32)
  file(GLOB MSW_SRCS_G "${CMAKE_CURRENT_SOURCE_DIR}/msw/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/msw/*.h*")
else()
  file(GLOB UNIX_SRCS_G "${CMAKE_CURRENT_SOURCE_DIR}/unix/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/unix/*.h*")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")


add_shared(instrument INSTRUMENT_API ${SRCS_G} ${MSW_SRCS_G} ${UNIX_SRCS_G})
target_compile_features(instrument PRIVATE cxx_std_20)
foreach(stacktrace stacktrace_windbg stacktrace_backtrace stacktrace_addr2line stacktrace_basic stacktrace_noop)
  find_package(Boost CONFIG COMPONENTS ${stacktrace})
  if (TARGET Boost::${stacktrace})
    target_link_libraries(instrument PUBLIC Boost::${stacktrace})  
    break()
  endif()
endforeach()



target_compile_definitions(instrument PRIVATE BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED=1 FMT_HEADER_ONLY=1 MODULE_NAME="instrument")
target_link_libraries(instrument PRIVATE easy_log)
setTartetBasic(instrument
    
)
endif()
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} TYPE INCLUDE FILES_MATCHING PATTERN "*.cmake")
