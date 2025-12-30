if(TARGET aes_Test)
    find_package(OpenSSL 1.1 REQUIRED)
    target_link_libraries(aes_Test PRIVATE utility OpenSSL::SSL OpenSSL::Crypto)
endif()
