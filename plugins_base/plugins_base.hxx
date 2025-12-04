#pragma once
#include <boost/preprocessor/cat.hpp>

#include <stdint.h>
namespace Anycubic::Plugins {
struct Plugin;
struct PluginHost;
} // namespace Anycubic::Plugins

constexpr int PLUGIN_API_VERSION = 1;
#define PLUGIN_EXPORT_GETINFO(export, plugin_name)                             \
  export PluginInfo *BOOST_PP_CAT(plugin_name, _getInfo)()
#define PLUGIN_EXPORT_SETUP(export, plugin_name)                               \
  export struct Anycubic::Plugins::Plugin *BOOST_PP_CAT(plugin_name, _setup)(  \
      struct Anycubic::Plugins::PluginHost * host)

extern "C" {

/**
 * @brief 插件信息结构体
 * @note 插件自身信息描述
 *
 */
struct PluginInfo {
  int16_t plugin_api;       ///< 插件api版本
  int16_t plugin_type;      ///< 插件类型:1: core 模块;2 dlg_widgets; 4: sdk; 8:
                            ///< webview_widgets。
  int32_t dependency_count; ///< 插件依赖数量
  const char **dependency;  ///< 插件依赖
  const char *name;         ///< 插件名称
  const char *description;  ///< 插件描述
  int64_t version;          ///< 插件版本
};
typedef PluginInfo *(*pluginGetInfo)();
typedef struct Plugin *(*pluginSetup)(struct PluginHost *host);
} // extern "C"
