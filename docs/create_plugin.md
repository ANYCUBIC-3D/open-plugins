# 创建插件


## 快速开始

### 环境要求

- CMake 3.25+（cmake 4未测试）
- C++20 兼容编译器
- wxWidgets (AnycubicSlicerNext依赖版本)
- 支持的操作系统：Windows、macOS、Linux（未测试）



## 插件开发

### 插件SDK核心模块

#### 1. 事件系统 (plugins_sdk/event/)
- `wxPluginEvent`: 通用插件事件包装器
- 支持键值对数据传递
- 共享数据指针管理
- 状态和错误码管理

#### 2. WebView API系统 (plugins_sdk/webview/)
- `WebviewApiHandler`: Web API处理器
- `RouterBase`: 路由基础类
- `InstanceBase`: 业务实例基类
- 支持GET/POST/DELETE/PUT方法

### 创建新插件

#### 步骤1: 创建插件目录结构

#### 步骤2: 编写插件CMakeLists.txt
```cmake
find_package(OpenPlugins CONFIG COMPONENTS plugins_manager plugins_base webview_sdk event_sdk easy_log utility ACWebView constant)
if(OpenPlugins_FOUND)
find_package(wxWidgets 3.1 REQUIRED COMPONENTS core base adv xrc)
include(${wxWidgets_USE_FILE})

function(add_shared_plugin target)
    add_shared(${target} ${target}_API ${ARGN})
    target_link_libraries(${target} PRIVATE  OpenPlugins::constant OpenPlugins::plugins_manager OpenPlugins::plugins_base OpenPlugins::webview_sdk OpenPlugins::ACWebView OpenPlugins::event_sdk OpenPlugins::easy_log OpenPlugins::utility ${wxWidgets_LIBRARIES})
    target_compile_definitions(${target} PRIVATE MODULE_NAME="${target}" ENABLE_STRACE=1  PLUGIN_NAME=${target}  wxDEBUG_LEVEL=0)
    set_target_properties(${target} PROPERTIES 
        FOLDER "Plugins/Shared"
        OUTPUT_NAME plugin_${target}
    )
    target_compile_features(${target} PRIVATE cxx_std_20)
   
    get_target_property(target_source_dir ${target} SOURCE_DIR)
    get_target_property(target_name ${target} NAME)
    set(POT_FILE ${target_source_dir}/${target_name}/${target_name}.pot)
    i18n_generate_pot(${target} ${POT_FILE} ${target_source_dir}/${target_name}/i18n/ zh_CN)
    i18n_generate_mo(${target_source_dir}/${target_name}/i18n/)
    i18n_generate_embed(${target} ${target_source_dir}/${target_name}/i18n/)
endfunction()

# 创建插件编译配置
file(GLOB_RECURSE PLUGIN_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.h*")
add_shared_plugin(address ${PLUGIN_SOURCES})
find_package(Boost CONFIG REQUIRED json)
target_link_libraries(address PUBLIC common_httpClient ${CURL_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto Boost::json)
```

#### 步骤3: 实现插件类
```cpp
#include "address_export.hxx"
#include "plugins_base/plugins_base.hxx"

#pragma once

#include <plugins_base/plugins.hxx>

#include <boost/preprocessor/stringize.hpp>

#include <string>

#define PLUGIN_NAME_STR BOOST_PP_STRINGIZE(PLUGIN_NAME) // cmake配置的宏{PLUGIN_NAME}

namespace Anycubic::Plugins {

class AddressPlugin : public Plugin {
public:
  AddressPlugin(PluginHost *host);
  ~AddressPlugin();

private:
  std::string get_cloud_url(void) const;


private:
  // Plugin 接口实现
  const char *Name(void) override { return PLUGIN_NAME_STR; };
  bool Start(void) override { return true; }
  void Stop(void) override {}
  bool AttachEvt(wxEvtHandler *evt) override;
  bool DetachEvt(wxEvtHandler *evt) override;
  bool BindEvt(wxPanel *panel, wxWindow *parent = nullptr,
               wxString *bmp = nullptr) override;
  bool CreateWebview(wxWebView *view, wxWindow *parent = nullptr,
                     wxString *bmp = nullptr) override;
  void Destroy(void) override;
private:
  PluginHost *host_;
};
} // namespace Anycubic::Plugins

#include <plugins_base/funcation.hxx>

namespace Anycubic::Plugins {
AddressPlugin::AddressPlugin(PluginHost *host) : host_(host) {
  assert(host_ != nullptr);
  auto router = host_->Router();
  assert(router != nullptr);

  router->REGISTER_FUNCATION(AddressPlugin, get_cloud_url);
}

AddressPlugin::~AddressPlugin() {}

std::string AddressPlugin::get_cloud_url(void) const {
  return "https://xxx";
}

bool AddressPlugin::AttachEvt(wxEvtHandler *evt) { return false; }

bool AddressPlugin::DetachEvt(wxEvtHandler *evt) { return false; }

bool AddressPlugin::BindEvt(wxPanel *panel, wxWindow *parent, wxString *bmp) {
  return false;
}

bool AddressPlugin::CreateWebview(wxWebView *view, wxWindow *parent,
                                  wxString *bmp) {
  return false;
}

void AddressPlugin::Destroy(void) { delete this; }

} // namespace Anycubic::Plugins

extern "C" {
PLUGIN_EXPORT_GETINFO(address_API, PLUGIN_NAME) {
  static const char *depends[] = {"utility"}; // 依赖的插件列表，注意一定要静态变量
  static PluginInfo info{
      .plugin_api = 1,
      .plugin_type = 0xe,
      .dependency_count = sizeof(depends) / sizeof(depends[0]),
      .dependency = depends,
      .name = PLUGIN_NAME_STR,
      .description = PLUGIN_NAME_STR,
      .version = 1,
  };
  return &info;
}
PLUGIN_EXPORT_SETUP(address_API, PLUGIN_NAME) {
  return new Anycubic::Plugins::AddressPlugin(host);
}
} // extern "C"
```


## 故障排除

### 常见问题

1. **插件加载失败**
   - 检查依赖项是否满足
   - 验证插件文件完整性
   - 查看系统日志

2. **API调用错误**
   - 检查参数类型和数量
   - 验证权限设置
   - 查看网络连接

### 获取帮助
- 查看详细文档
- 提交Issue到仓库
- 参与社区讨论

---

# Creating Plugins

## Quick Start

### Environment Requirements

- CMake 3.25+ (cmake 4 untested)
- C++20 compatible compiler
- wxWidgets (AnycubicSlicerNext dependency version)
- Supported operating systems: Windows, macOS, Linux (not tested)

## Plugin Development

### Plugin SDK Core Modules

#### 1. Event System (plugins_sdk/event/)
- `wxPluginEvent`: Universal plugin event wrapper
- Supports key-value pair data transfer
- Shared data pointer management
- Status and error code management

#### 2. WebView API System (plugins_sdk/webview/)
- `WebviewApiHandler`: Web API processor
- `RouterBase`: Router base class
- `InstanceBase`: Service instance base class
- Supports GET/POST/DELETE/PUT methods

### Creating a New Plugin

#### Step 1: Create Plugin Directory Structure

#### Step 2: Write Plugin CMakeLists.txt
```cmake
find_package(OpenPlugins CONFIG COMPONENTS plugins_manager plugins_base webview_sdk event_sdk easy_log utility ACWebView constant)
if(OpenPlugins_FOUND)
find_package(wxWidgets 3.1 REQUIRED COMPONENTS core base adv xrc)
include(${wxWidgets_USE_FILE})

function(add_shared_plugin target)
    add_shared(${target} ${target}_API ${ARGN})
    target_link_libraries(${target} PRIVATE  OpenPlugins::constant OpenPlugins::plugins_manager OpenPlugins::plugins_base OpenPlugins::webview_sdk OpenPlugins::ACWebView OpenPlugins::event_sdk OpenPlugins::easy_log OpenPlugins::utility ${wxWidgets_LIBRARIES})
    target_compile_definitions(${target} PRIVATE MODULE_NAME="${target}" ENABLE_STRACE=1  PLUGIN_NAME=${target}  wxDEBUG_LEVEL=0)
    set_target_properties(${target} PROPERTIES 
        FOLDER "Plugins/Shared"
        OUTPUT_NAME plugin_${target}
    )
    target_compile_features(${target} PRIVATE cxx_std_20)
   
    get_target_property(target_source_dir ${target} SOURCE_DIR)
    get_target_property(target_name ${target} NAME)
    set(POT_FILE ${target_source_dir}/${target_name}/${target_name}.pot)
    i18n_generate_pot(${target} ${POT_FILE} ${target_source_dir}/${target_name}/i18n/ zh_CN)
    i18n_generate_mo(${target_source_dir}/${target_name}/i18n/)
    i18n_generate_embed(${target} ${target_source_dir}/${target_name}/i18n/)
endfunction()

# Create plugin compilation configuration
file(GLOB_RECURSE PLUGIN_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.h*")
add_shared_plugin(address ${PLUGIN_SOURCES})
find_package(Boost CONFIG REQUIRED json)
target_link_libraries(address PUBLIC common_httpClient ${CURL_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto Boost::json)
```

#### Step 3: Implement Plugin Class
```cpp
#include "address_export.hxx"
#include "plugins_base/plugins_base.hxx"

#pragma once

#include <plugins_base/plugins.hxx>

#include <boost/preprocessor/stringize.hpp>

#include <string>

#define PLUGIN_NAME_STR BOOST_PP_STRINGIZE(PLUGIN_NAME) // Macro configured in cmake {PLUGIN_NAME}

namespace Anycubic::Plugins {

class AddressPlugin : public Plugin {
public:
  AddressPlugin(PluginHost *host);
  ~AddressPlugin();

private:
  std::string get_cloud_url(void) const;


private:
  // Plugin interface implementation
  const char *Name(void) override { return PLUGIN_NAME_STR; };
  bool Start(void) override { return true; }
  void Stop(void) override {}
  bool AttachEvt(wxEvtHandler *evt) override;
  bool DetachEvt(wxEvtHandler *evt) override;
  bool BindEvt(wxPanel *panel, wxWindow *parent = nullptr,
               wxString *bmp = nullptr) override;
  bool CreateWebview(wxWebView *view, wxWindow *parent = nullptr,
                     wxString *bmp = nullptr) override;
  void Destroy(void) override;
private:
  PluginHost *host_;
};
} // namespace Anycubic::Plugins

#include <plugins_base/funcation.hxx>

namespace Anycubic::Plugins {
AddressPlugin::AddressPlugin(PluginHost *host) : host_(host) {
  assert(host_ != nullptr);
  auto router = host_->Router();
  assert(router != nullptr);

  router->REGISTER_FUNCATION(AddressPlugin, get_cloud_url);
}

AddressPlugin::~AddressPlugin() {}

std::string AddressPlugin::get_cloud_url(void) const {
  return "https://xxx";
}

bool AddressPlugin::AttachEvt(wxEvtHandler *evt) { return false; }

bool AddressPlugin::DetachEvt(wxEvtHandler *evt) { return false; }

bool AddressPlugin::BindEvt(wxPanel *panel, wxWindow *parent, wxString *bmp) {
  return false;
}

bool AddressPlugin::CreateWebview(wxWebView *view, wxWindow *parent,
                                  wxString *bmp) {
  return false;
}

void AddressPlugin::Destroy(void) { delete this; }

} // namespace Anycubic::Plugins

extern "C" {
PLUGIN_EXPORT_GETINFO(address_API, PLUGIN_NAME) {
  static const char *depends[] = {"utility"}; // List of dependent plugins, must be static variables
  static PluginInfo info{
      .plugin_api = 1,
      .plugin_type = 0xe,
      .dependency_count = sizeof(depends) / sizeof(depends[0]),
      .dependency = depends,
      .name = PLUGIN_NAME_STR,
      .description = PLUGIN_NAME_STR,
      .version = 1,
  };
  return &info;
}
PLUGIN_EXPORT_SETUP(address_API, PLUGIN_NAME) {
  return new Anycubic::Plugins::AddressPlugin(host);
}
} // extern "C"
```

## Troubleshooting

### Common Issues

1. **Plugin Loading Failure**
   - Check if dependencies are satisfied
   - Verify plugin file integrity
   - Check system logs

2. **API Call Errors**
   - Check parameter types and values
   - Verify permission settings
   - Check network connection

### Getting Help
- View detailed documentation
- Submit issues to the repository
- Participate in community discussions