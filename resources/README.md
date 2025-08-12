# 使用说明

## 创建CMakeLists.txt

* 必须将依赖的cmake目录添加到CMAKE_MODULE_PATH中，否则无法找到依赖的模块。
* 创建一个空的库文件，并包含GenerateExportHeader模块。
  * 使用generate_export_header命令生成导出头文件。
  * 导致头文件只能是`resources_export.hxx`

```cmake
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # 包含生成的头文件目录
add_library(PACAGE empty.c) # empty.c文件可以为空
include(GenerateExportHeader)
generate_export_header(PACAGE EXPORT_MACRO_NAME RESOURCES_EXPORT EXPORT_FILE_NAME resources_export.hxx)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake") # 添加依赖的cmake目录（根据实施情况写）
```

## 导入实现

```cmake
include(build)
include(rc)
include(generate)
```

## 编译资源dll

```cmake
# CMAKE_CURRENT_SOURCE_DIR 变量替换为resources资源目录，为了找到其中`common.hpp` 文件
# CMAKE_CURRENT_BINARY_DIR 变量替换为build目录，为了生成资源dll相关的源文件放置目录
# INSTALL_PREFIX 变量替换为安装目录，为了生成资源dll放置目录
# PACKAGES_LIST 变量替换为资源dll依赖的压缩包列表
generate_source_file(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/generate ${INSTALL_PREFIX} ${PACKAGES_LIST})

```