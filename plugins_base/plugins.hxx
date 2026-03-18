// Copyright (c) 2026 深圳市纵维立方科技有限公司
// Open-Plugin is licensed under Mulan PSL v2.
// You can use this software according to the terms and conditions of the Mulan
// PSL v2.
// You may obtain a copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
// See the Mulan PSL v2 for more details.

#pragma once
#include <stddef.h>
#include <stdint.h>

class wxEvtHandler;
class wxDialog;
class wxWindow;
class wxWebView;
class wxBitmap;
class wxString;
class wxPanel;

namespace Anycubic::Plugins {
struct IStream;
struct OStream;

/**
 * @brief 请求处理器接口
 *
 */
struct RequestHandler {
  virtual ~RequestHandler() = default;
  /**
   * @brief 执行插件请求处理
   * @param data 输入数据流
   * @param result 输出结果流
   */
  virtual void Execute(IStream *data, OStream *result) = 0;

  /**
   * @brief 销毁处理器资源
   */
  virtual void Destroy() = 0;
};

/**
 * @brief 插件路由接口
 * @note 插件路由接口，用于管理插件函数的注册和执行
 *
 */
struct PluginRouter {
  virtual ~PluginRouter() = default;
  /**
   * @brief 添加插件函数处理器
   *
   * @param fname 函数名称
   * @param handler 请求处理器对象
   * @return true 添加成功
   * @return false 添加失败
   */
  virtual bool AddFunction(const char *fname, RequestHandler *handler) = 0;

  /**
   * @brief 执行插件函数
   *
   * @param plugin 插件名称
   * @param fname 函数名称
   * @param data 输入数据流
   * @param result 输出结果流
   * @return true 执行成功
   * @return false 执行失败
   */
  virtual bool ExecuteFunction(const char *plugin, const char *fname,
                               struct IStream *data,
                               struct OStream *result) = 0;
};

/**
 * @brief 插件接口
 * @note 插件接口，定义了插件的基本功能和生命周期管理
 *
 */
struct Plugin {
  virtual ~Plugin() = default;
  /**
   * @brief 插件名称
   *
   * @return const char* 插件名称字符串
   */
  virtual const char *Name(void) = 0;
  /**
   * @brief 插件启动函数
   *
   * @return true 启动成功
   * @return false 启动失败
   */
  virtual bool Start(void) = 0;

  /**
   * @brief 插件停止函数
   *
   */
  virtual void Stop(void) = 0;

  /**
   * @brief 附加事件处理器
   *
   * @param evt 事件处理对象
   * @return true 附加成功
   * @return false 附加失败
   */
  virtual bool AttachEvt(wxEvtHandler *evt) = 0;
  /**
   * @brief 分离事件处理器
   * @param evt 要分离的事件处理对象
   * @return true 分离成功
   * @return false 分离失败
   */
  virtual bool DetachEvt(wxEvtHandler *evt) = 0;

  /**
   * @brief 绑定事件
   * @param panel 面板对象
   * @param parent 父窗口指针
   * @param bmp 位图资源指针
   * @return true 创建成功
   * @return false 创建失败
   */
  virtual bool BindEvt(wxPanel *panel, wxWindow *parent = nullptr,
                       wxString *bmp = nullptr) = 0;
  /**
   * @brief 创建插件Webview
   *
   * @param view Webview对象
   * @param parent 父窗口指针
   * @param bmp 位图资源指针
   * @return true 创建成功
   * @return false 创建失败
   */
  virtual bool CreateWebview(wxWebView *view, wxWindow *parent = nullptr,
                             wxString *bmp = nullptr) = 0;

  /**
   * @brief 销毁插件资源
   */
  virtual void Destroy(void) = 0;
};

/**
 * @brief 插件主机接口
 * @note 插件主机接口，定义了插件与主机环境的交互接口
 *
 */
struct PluginHost {
  virtual ~PluginHost() = default;
  /**
   * @brief 获取路由管理器
   *
   * @return PluginRouter* 路由管理器实例
   */
  virtual PluginRouter *Router(void) = 0;

  /**
   * @brief 检查插件是否存在
   *
   * @param name 插件名称
   * @return true 存在
   * @return false 不存在
   */
  virtual bool HasPlugin(const char *name) = 0;

  /**
   * @brief 获取指定插件实例
   * @param name 插件名称
   * @return Plugin* 插件实例指针
   */
  virtual Plugin *GetPlugin(const char *name) = 0;

  /**
   * @brief Get the Window object
   *
   * @param postion 窗口位置
   * @return wxWindow* 窗口指针
   */
  virtual wxWindow *GetWindow(const char *postion = nullptr) = 0;

  /**
   * @brief 监听窗口创建事件
   *
   * @param postion 窗口位置
   * @param context 上下文指针
   * @param callback 回调函数指针 --
   * 当callback返回false时将影响AddWidget的存储行为为不存储
   * @return true 监听成功
   * @return false 监听失败
   */
  virtual bool WatchWindow(const char *postion, void *context,
                           bool (*callback)(void *context,
                                            wxWindow *window)) = 0;
  /**
   * @brief Create a Panel object
   *
   * @param position 父容器位置标识符，用于定位添加位置
   * @param xrcName 插件提供的XRC资源名称，用于加载面板窗口
   * @param xrc 插件提供的XRC资源字符串，用于创建面板窗口
   * @return wxWindow*  创建的面板窗口指针
   */
  virtual wxPanel *CreatePanel(const wxString &position,
                               const wxString &xrcName,
                               const wxString &xrc) = 0;
  /**
   * @brief Create a Panel object
   *
   * @param parent 父容器窗口指针，用于添加面板窗口
   * @param xrcName 插件提供的XRC资源名称，用于加载面板窗口
   * @param xrc 插件提供的XRC资源字符串，用于创建面板窗口
   * @return wxWindow*  创建的面板窗口指针
   */
  virtual wxPanel *CreatePanel(wxWindow *parent, const wxString &xrcName,
                               const wxString &xrc) = 0;
  /**
   * @brief 获取当前语言
   *
   * @return wxString 当前语言名称, 例如 "zh_CN"
   */
  virtual wxString Language(void) = 0;

  /**
   * @brief 加载翻译文件
   *
   * @param domain
   * 翻译有效域，这个只是为了兼容wxWidgets的翻译机制，并不实际使用, eg:
   * "zh_CN",如果不确认定用wxEmptyString
   * @param data 翻译文件数据指针
   * @param bytes 数据长度
   * @return true 加载成功
   * @return false 加载失败
   */
  virtual bool LoadTranslationFromData(const wxString &domain, void *data,
                                       size_t bytes) = 0;

  /**
   * @brief 从文件加载翻译
   *
   * @param domain 翻译域, 这个只是为了兼容wxWidgets的翻译机制，并不实际使用,
   * eg: "zh_CN",如果不确认定用wxEmptyString
   * @param path 翻译文件路径, eg:
   * "/usr/share/locale/zh_CN/LC_MESSAGES/anycubic.mo"
   * @return true 加载成功
   * @return false 加载失败
   */
  virtual bool LoadTranslationFromFile(const wxString &domain,
                                       const wxString &path) = 0;

  /**
   * @brief  添加虚拟文件系统
   *
   * @param name 虚拟文件系统名称
   * @param xrc 文件内容
   * @return true 添加成功
   * @return false 添加失败
   */
  virtual bool AddFS(const wxString &name, const wxString &xrc) = 0;

  /**
   * @brief 删除虚拟文件系统
   *
   * @param name 虚拟文件系统名称
   * @return true 删除成功
   * @return false 删除失败
   */
  virtual bool DelFS(const wxString &name) = 0;
  /**
   * @brief 添加虚拟文件系统
   * @param name 文件系统名称
   * @param data 文件数据指针
   * @param length 数据长度
   * @return true 添加成功
   * @return false 添加失败
   */
  virtual bool AddFS(const wxString &name, void *data, size_t length) = 0;

  /**
   * @brief 获取配置项
   * @param key 配置键值，支持两级结构如 "section/key"
   * @param value [输出] 配置值
   */
  virtual bool GetValue(const wxString &, wxString &value) = 0;

  /**
   * @brief 设置配置项
   * @param persistent 是否持久化存储，默认true
   */
  virtual bool SetValue(const wxString &, const wxString &value,
                        bool persistent = true) = 0;

  /**
   * @brief 获取加密配置项
   * @note 实现类应负责加解密逻辑
   */
  virtual bool GetEncryptValue(const wxString &, wxString &value) = 0;

  /**
   * @brief 设置加密配置项
   */
  virtual bool SetEncryptValue(const wxString &, const wxString &value,
                               bool persistent = true) = 0;

  /**
   * @brief 释放配置项值内存
   *
   * @param value 配置项值指针
   */
  virtual void Free(const char *value) = 0;
};
} // namespace Anycubic::Plugins