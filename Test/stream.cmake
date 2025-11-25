if(TARGET stream_Test)
    find_package(wxWidgets 3.1 COMPONENTS core base adv)
    target_link_libraries(stream_Test PRIVATE ${wxWidgets_LIBRARIES} plugins_base plugins_manager)
endif()
