# 插件化架构基础库 - 目录结构说明
# Plugin-based Architecture Library - Directory Structure

## 📖 项目概述 | Project Overview
这是一个基于 wxWidgets 开发的插件化架构基础功能库，为桌面应用提供完整的插件化解决方案。核心能力包括：
- 插件的动态加载、卸载与生命周期管理
- wxWebView 组件集成（支持 Windows/macOS/Linux）
- 插件事件系统与通信机制
- 完善的插件开发 SDK 体系

This is a plugin-based architecture library built on wxWidgets, providing a complete plugin solution for desktop applications with core capabilities:
- Dynamic loading, unloading and lifecycle management of plugins
- wxWebView component integration (Windows/macOS/Linux)
- Plugin event system and communication mechanism
- Comprehensive SDK for plugin development

## 📂 项目结构 | Project Structure

```
├── 3rdparty/                          # 第三方依赖库 | Third-party dependencies
│   ├── fmt/                           # 高性能C++格式化库 | High-performance formatting library
│   ├── iguana/                        # C++序列化/反序列化库 | Serialization/deserialization library
│   ├── nlohmann/                      # Modern C++ JSON库 | JSON for Modern C++
│   └── spdlog/                        # 高效日志库 | High-performance logging library
│
├── WebView/                           # wxWebView组件模块 | wxWebView Component
│
├── cmake/                             # CMake构建系统配置 | CMake build system
│   ├── GenerateTarget.cmake.in        # 目标生成模板 | Target generation template
│   ├── OpenPluginsConfig.cmake.in     # 项目配置模板 | Project config template
│   ├── subdir.cmake                   # 子目录配置 | Subdirectory config
│   ├── generate.cmake                 # 构建生成脚本 | Generation scripts
│   ├── sources.cpp.in                 # 源码生成模板 | Source generation template
│   ├── FindwxWidgets.cmake            # wxWidgets查找配置 | wxWidgets find config
│   └── Utils.cmake                    # CMake工具函数 | CMake utility functions
│
├── docs/                              # 项目文档 | Documentation
│   ├── directory.md                   # 目录结构说明 | Directory structure docs
│   ├── build_guide.md                 # 构建指南 | Build guide
│   ├── plugin_development_guide.md    # 插件开发指南 | Plugin development guide
│   └── api_reference.md               # API参考文档 | API reference
│
├── easy_log/                          # 简易日志模块（基于spdlog） | Lightweight logging module（base on spdlog）
│   ├── easy_log.hxx                   # 日志公共接口 | Log public interface
│   ├── log_config.hxx                 # 日志配置 | Log config 
│
├── plugins/                           # 测试插件示例 | Test plugins
│   ├── README.md                      # 插件目录说明 | Plugin directory docs
│   ├── app/                           # 应用类插件示例 | Application plugin examples
│   └── utility/                       # 工具类插件示例 | Utility plugin examples
│
├── plugins_base/                      # 插件基础模块 | Plugin base module
│   ├── plugins.hxx                # 核心插件接口 | Core plugin interface
│   ├── plugins_base.hxx           # 基础定义 | Basic definitions
│   ├── i18n.hxx                   # 国际化支持 (从AnycubicSlicerNext复制)| Internationalization support（from AnycubicSlicerNext）
│   ├── i18n_helper.hxx            # 翻译文件打包支持定义 | Translation file packaging support definition
│   ├── funcation.hxx              # 成员函数包装 | Member function wrapper
│   ├── function_traits.hxx        # 成员函数特征提取 | Member function traits
│   ├── tuple_foreach.hxx          # 元组遍历辅助函数 | Tuple foreach helper function
│   └── plugins_stream             # 插件流操作定义 | Plugin stream definitions
│
├── plugins_manager/                   # 插件管理器（核心） | Plugin Manager (Core)
│       ├── plugins_manager            # 管理器接口 | Manager interface
│       └── detail/                    # 内部实现细节 | Internal implementation
│           └── xrc/                   # xrc 相关实现 | XRC related implementation
│           │   ├── xh_webview         # wxWebView xrc支持 | wxWebView xrc support 
│           ├── plugins_manager_impl   # 实现头文件 | Implementation 
│           ├── package                # 插件包定义 | Plugin package definition
│           ├── LibraryBase            # 库基础类 | Library base class
│           ├── router                 # 函数调用路由 | Function router
│           ├── tranclations           # i18n 框架接入实现 | I18N translations
│           └── fshandler              # 文件系统处理器 | File system handler
│
├── plugins_sdk/                       # 插件开发SDK | Plugin Development SDK
│   ├── constant/                      # 通用常量定义 | Constant definitions
│   │   ├── config                     # 配置常量 | Config definitions
│   ├── event/                         # 事件系统 | Event system
│   │   ├── plugin_event               # 插件事件类型定义 | Plugin event type definitions
│   │   ├── dep_copy.hxx               # 结构体深度复制 | Struct deep copy
│   │   └── detail/                    # 详细实现 | Event system detail
│   │       ├── cloud_event            # 云事件列表定义 | Cloud event definitions
│   │       ├── cloud_mqtt_event       # 云MQTT事件类型定义 | Cloud MQTT event type definitions
│   │       └── mach_mqtt_event        # 机器MQTT事件类型定义 | Machine MQTT event type definitions
│   └── webview/                       # WebView操作API | WebView operation APIs
│       ├── webview_api_handler        # wxWebView API 处理器 | wxWebView API handler
│       ├── webview_rc_handler         # WebView资源处理器 | WebView resource handler
│       ├── object_base.hxx            # 抽象类定义 | WebView object base class
│       └── detail/                    # 详细实现 | Event system detail
│           ├── router                 # 路由定义 | Router definitions
│           └── script_format.hxx      # 脚本格式化实现和调试支持实现 | Script format definitions
│
├── resources/                         # 资源管理 | Resource management
│   └── common.hxx                     # 公共定义 | Common definitions
│
├── Test/                              # 单元测试 | Unit Tests
│   ├── CMakeLists.txt                 # 测试构建配置 | Test build config
│   ├── plugins_manager_Test.cpp       # 插件管理器测试 | Plugin manager tests
│   ├── stream_Test.cpp                # 流操作测试 | Stream operation tests
│   ├── aes_Test.cpp                   # AES加密测试 | AES encryption tests
│   ├── webview_Test.cpp               # WebView功能测试 | WebView function tests
│   ├── event_Test.cpp                 # 事件系统测试 | Event system tests
│   ├── plugins_manager.cmake          # 插件管理器测试配置 | Test configuration
│   ├── stream.cmake                   # 流测试配置 | Stream test config
│   ├── aes.cmake                      # AES测试配置 | AES test config
│   ├── webview.cmake                  # WebView测试配置 | WebView test config
│   └── event.cmake                    # 事件测试配置 | Event test config
│
└── utility/                           # 通用工具库 | Utility Library
    ├── codec/                         # 编解码工具 | Codec utilities
    │   ├── bin2ascii                  # 二进制转ASCII | Binary to ASCII
    │   ├── base64                     # Base64编解码 | Base64 codec
    │   └── md5                        # MD5算法 | MD5 algorithm
    ├── encrypt/                       # 加密工具 | Encryption utilities
    │   ├── aes                        # AES加密 | AES encryption
    │   └── rsa                        # RSA加密 | RSA encryption
    ├── file/                          # 文件工具 | File utilities
    │   ├── file_utils.hxx             # 文件操作工具 | File operation utils
    │   └── path_utils.hxx             # 路径工具 | Path utils
    └── utils/                         # 集合  | Collection utilities
        ├── filesystem                 # 文件系统  | File system utils
        ├── query                      # 查询参数解析  | 
        ├── string                     # 字符串 | String utils
        └── range.hxx                  # 范围for简化 | Range for simplification
```

## 🎯 核心模块功能说明 | Core Module Descriptions

| 模块目录 | 核心功能 | Core Functions |
|----------|----------|----------------|
| 3rdparty/ | 依赖的第三方基础库，包含格式化、序列化、JSON、日志等核心能力 | Dependencies of third-party libraries for formatting, serialization, JSON, logging |
| WebView/ | wxWebView组件<br>默认wxwidgets自带的太老，也不方便定制修改 | wxWebView component<br>Default wxWidgets built-in is too old and not convenient for customization |
| plugins_base/ | 定义插件体系的基础接口和数据结构，包括插件信息、核心接口、API版本等 | Defines basic interfaces and data structures for plugins (PluginInfo, Plugin interface, API version) |
| plugins_manager/ | 插件系统核心，负责插件的动态加载/卸载、生命周期管理、事件路由 | Core of plugin system - dynamic loading/unloading, lifecycle management, event routing |
| plugins_sdk/ | 面向插件开发者的SDK，提供事件系统和WebView操作API | SDK for plugin developers, providing event system and WebView operation APIs |
| Test/ | 单元测试 | Unit tests  |
| utility/ | 通用工具库，提供编解码（MD5）、加密（AES）等基础工具函数 | Utility library with codec (MD5) and encryption (AES) functions |