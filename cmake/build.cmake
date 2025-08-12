find_package(Git REQUIRED)



macro(get_build_info _git_hash _git_branch _build_time)   #
    execute_process(          # 执行一个子进程
        COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h # 命令
        OUTPUT_VARIABLE ${_git_hash}        # 输出字符串存入变量
        OUTPUT_STRIP_TRAILING_WHITESPACE    # 删除字符串尾的换行符
        ERROR_QUIET                         # 对执行错误静默
        WORKING_DIRECTORY                   # 执行路径
          ${CMAKE_SOURCE_DIR}
        )
    execute_process(          # 执行一个子进程
        COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD # 命令
        OUTPUT_VARIABLE ${_git_branch}        # 输出字符串存入变量
        OUTPUT_STRIP_TRAILING_WHITESPACE    # 删除字符串尾的换行符
        ERROR_QUIET                         # 对执行错误静默
        WORKING_DIRECTORY                   # 执行路径
          ${CMAKE_SOURCE_DIR}
        )

    string(TIMESTAMP ${_build_time} "%Y-%m-%d %H:%M:%S")
endmacro() 

function(generate_build_header filename)
    get_build_info(hash branch time)
    message(STATUS "build info:")
    message(STATUS "\t\tcommit hast: ${hash}")
    message(STATUS "\t\tbuild branch: ${branch}")
    message(STATUS "\t\tbuild time: ${time}")

    file(WRITE ${filename} "#pragma once\n")
    file(APPEND  ${filename}  "#define GIT_COMMIT_HAST \"${hash}\"\n")
    file(APPEND  ${filename}  "#define GIT_BRANCHE \"${branch}\"\n")
    file(APPEND  ${filename}  "#define BUILD_TIME \"${time}\"\n")
    file(APPEND  ${filename}  "#if defined(__WXMSW__) && !defined(NDEBUG)\n")
    file(APPEND  ${filename}  "#include <crtdbg.h>\n")
    file(APPEND  ${filename}  "#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)\n")
    file(APPEND  ${filename}  "#endif\n")
    file_install(xxx ${filename})
endfunction()