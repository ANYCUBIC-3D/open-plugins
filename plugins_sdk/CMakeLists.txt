file(GLOB_RECURSE SUB_PLUGIN "${CMAKE_CURRENT_LIST_DIR}/*.cmake")


foreach(plugin ${SUB_PLUGIN})
    include(${plugin})
endforeach()

