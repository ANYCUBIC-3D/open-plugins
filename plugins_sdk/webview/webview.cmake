file(GLOB_RECURSE SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.h*")
file(GLOB_RECURSE HDRS  "${CMAKE_CURRENT_LIST_DIR}/*.hxx")

find_package(wxWidgets 3.3 REQUIRED CONFIG COMPONENTS core base adv)
include(${wxWidgets_USE_FILE})
add_static(webview_sdk WEBVIEW_EXPORT ${SRC}) 
setTartetBasic(webview_sdk 
LIBRARIES ACWebView easy_log iguana nlohmann_json ${wxWidgets_LIBRARIES}
) 
target_compile_definitions(webview_sdk PRIVATE wxDEBUG_LEVEL=0 MODULE_NAME="" ENABLE_STRACE=1 BUILD_WEBVIEW_API=1)
set_target_properties(webview_sdk PROPERTIES FOLDER "PluginsSDK")
target_include_directories(webview_sdk PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# foreach(DIR ${HDRS})
#     string(REPLACE "${CMAKE_CURRENT_LIST_DIR}" "" sub ${DIR})
#     cmake_path(GET sub PARENT_PATH sub)
#     module_install(${OS_PREFIX}/include/plugins_sdk/webview${sub} ${DIR})
# endforeach()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} TYPE INCLUDE FILES_MATCHING PATTERN "*.hxx")