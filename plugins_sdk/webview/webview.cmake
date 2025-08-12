file(GLOB_RECURSE SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.h*")
file(GLOB_RECURSE HDRS  "${CMAKE_CURRENT_LIST_DIR}/*.hxx")


add_static(webview_sdk WEBVIEW_EXPORT ${SRC}) 
setTartetBasic(webview_sdk 
LIBRARIES ACWebView easy_log iguana nlohmann_json
) 
target_compile_definitions(webview_sdk PRIVATE wxDEBUG_LEVEL=0 MODULE_NAME="" ENABLE_STRACE=1)
set_target_properties(webview_sdk PROPERTIES FOLDER "PluginsSDK")

# foreach(DIR ${HDRS})
#     string(REPLACE "${CMAKE_CURRENT_LIST_DIR}" "" sub ${DIR})
#     cmake_path(GET sub PARENT_PATH sub)
#     module_install(${OS_PREFIX}/include/plugins_sdk/webview${sub} ${DIR})
# endforeach()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} TYPE INCLUDE FILES_MATCHING PATTERN "*.hxx")