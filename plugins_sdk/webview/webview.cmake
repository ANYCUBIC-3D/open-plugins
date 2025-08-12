file(GLOB_RECURSE SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.h*")


add_static(webview_sdk WEBVIEW_EXPORT ${SRC}) 
setTartetBasic(webview_sdk 
LIBRARIES ACWebView easy_log
) 
target_compile_definitions(webview_sdk PRIVATE wxDEBUG_LEVEL=0 MODULE_NAME="" ENABLE_STRACE=1)
set_target_properties(webview_sdk PROPERTIES FOLDER "PluginsSDK")
