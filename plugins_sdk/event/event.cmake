find_package(wxWidgets 3.1 REQUIRED COMPONENTS core base adv)
include(${wxWidgets_USE_FILE})


file(GLOB_RECURSE SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.h*")
find_package(Boost CONFIG REQUIRED)
add_shared(event_sdk EVENT_API ${SRC}) 
setTartetBasic(event_sdk 
    LIBRARIES ${wxWidgets_LIBRARIES}
) 
target_include_directories(event_sdk PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_definitions(event_sdk PRIVATE ACPLUGIN_API_BUILD wxDEBUG_LEVEL=0 MODULE_NAME="" ENABLE_STRACE=1)
set_target_properties(event_sdk PROPERTIES FOLDER "PluginsSDK")

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} TYPE INCLUDE FILES_MATCHING PATTERN "*.hxx")