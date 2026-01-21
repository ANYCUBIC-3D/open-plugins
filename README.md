# OpenPlugins

OpenPlugins 是一个为 AnycubicSlicerNext 提供插件化基础功能的开源库。该项目通过提供插件开发所需的基础功能模块，简化了插件化开发流程，使开发者能够更便捷地构建和集成插件。

## 功能特性

* **模块化插件架构**：提供清晰的插件接口定义，支持插件的独立开发、部署和管理
* **统一通信协议**：基于内存流的数据传输机制，实现插件间的可靠通信
* **事件驱动模型**：支持事件监听和响应，便于插件间交互
* **WebView集成**：内置wxWebView支持
* **插件包管理**：支持插件打包、签名验证和版本管理
* **国际化支持**：内置多语言支持
* **XRC支持**: 支持使用XRC文件定义插件界面

## 文档

* [编译指南](docs/how_to_build.md)
* [创建插件](docs/create_plugin.md)
* [目录结构说明](docs/directory.md)
* [工作机制](docs/mechanism.md)

## 其他语言

* [English Version](README_EN.md)

## 贡献指南

欢迎社区开发者参与 OpenPlugins 的开发和维护工作。

## 贡献指南

* 请遵循我们的 [贡献指南](docs/contributing.md) 提交代码。
* 所有贡献都将根据 Mulan PSL v2 许可证进行授权。

## 许可证

本项目采用 [Mulan PSL v2](LICENSE) 许可证，详情请参阅 LICENSE 文件。