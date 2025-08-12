file(GLOB_RECURSE SUB_PLUGIN "*.cmake")



include_directories(${CMAKE_CURRENT_BINARY_DIR})




foreach(plugin ${SUB_PLUGIN})
    include(${plugin})
endforeach()
set(PM_DEPS "${PM_DEPS}" PARENT_SCOPE)


