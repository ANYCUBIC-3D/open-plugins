file(GLOB_RECURSE SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.h*")
file(GLOB_RECURSE HDRS  "${CMAKE_CURRENT_LIST_DIR}/*.hxx")


add_static(communication IPC_EXPORT ${SRC}) 
setTartetBasic(communication 
LIBRARIES  iguana nlohmann_json utility
) 
target_compile_definitions(communication PRIVATE wxDEBUG_LEVEL=0 MODULE_NAME="" ENABLE_STRACE=1 BUILD_IPC_API=1)
set_target_properties(communication PROPERTIES FOLDER "PluginsSDK")
target_include_directories(communication PRIVATE ${CMAKE_CURRENT_LIST_DIR})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} TYPE INCLUDE FILES_MATCHING PATTERN "*.hxx")