# 构建

## 基础步骤

1. 编译AnycubicSlicerNext的deps目录
2. 创建`CMakeUserPresets.json`文件,如下
    * **sourceDir** 为cmake的变量
    * **CMAKE_OSX_DEPLOYMENT_TARGET** 指定macos最低版本号
    * **ENABLE_TEST** 单元测试开关
    * **ENABLE_GTEST** gtest单元测试开关（可以使用vcpkg安装的gtest和配置使用）
    * **CMAKE_PREFIX_PATH** 需要指向deps编译安装目录

```json
{
    "version": 8,
    "configurePresets": [
        {
            "name": "macos_debug",
            "inherits": [
                "macos_base_arm64"
            ],
            "environment": {},
            "displayName": "XCode build debug",
            "description": "XCode debug",
            "binaryDir": "${sourceDir}/vscode_build_debug",
            "cacheVariables": {
                "CMAKE_OSX_DEPLOYMENT_TARGET": "13.0",
                "CMAKE_BUILD_TYPE": "Debug",
                "CMAKE_PREFIX_PATH": "${sourceDir}/AnycubicSlicerNext/install/Debug_arm64/usr/local;${sourceDir}/vcpkg/installed/arm64-osx",
                "ENABLE_TEST": "ON",
                "ENABLE_GTEST": "ON"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "macos_debug",
            "displayName": "XCode build debug",
            "description": "XCode debug",
            "configurePreset": "macos_debug"
        }
    ]
}
```

3. 配置编译项目

```shell
cmake --preset macos_debug # 配置
cmake --build build --preset macos_debug --verbose # 编译
```

## 其他说明

* **CMakePresets.json** 为多个平台都提供了默认配置，通过继承使用即可。

# Build

## Basic Steps

1. Compile the deps directory of AnycubicSlicerNext
2. Create the `CMakeUserPresets.json` file as follows:
    * **sourceDir** is a CMake variable
    * **CMAKE_OSX_DEPLOYMENT_TARGET** specifies the minimum macOS version
    * **ENABLE_TEST** unit test switch
    * **ENABLE_GTEST** gtest unit test switch (can use gtest installed via vcpkg and configure to use it)
    * **CMAKE_PREFIX_PATH** needs to point to the deps compilation installation directory

```json
{
    "version": 8,
    "configurePresets": [
        {
            "name": "macos_debug",
            "inherits": [
                "macos_base_arm64"
            ],
            "environment": {},
            "displayName": "XCode build debug",
            "description": "XCode debug",
            "binaryDir": "${sourceDir}/vscode_build_debug",
            "cacheVariables": {
                "CMAKE_OSX_DEPLOYMENT_TARGET": "13.0",
                "CMAKE_BUILD_TYPE": "Debug",
                "CMAKE_PREFIX_PATH": "${sourceDir}/AnycubicSlicerNext/install/Debug_arm64/usr/local;${sourceDir}/vcpkg/installed/arm64-osx",
                "ENABLE_TEST": "ON",
                "ENABLE_GTEST": "ON"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "macos_debug",
            "displayName": "XCode build debug",
            "description": "XCode debug",
            "configurePreset": "macos_debug"
        }
    ]
}
```

3. Configure and build the project

```shell
cmake --preset macos_debug # Configure
cmake --build build --preset macos_debug --verbose # Build
```

## Additional Notes

* **CMakePresets.json** provides default configurations for multiple platforms, which can be used through inheritance.