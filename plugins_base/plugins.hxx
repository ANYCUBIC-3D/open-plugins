#pragma once
#include <stddef.h>
#include <stdint.h>

class wxEvtHandler;
class wxDialog;
class wxWindow;
class wxWebView;
class wxBitmap;
class wxString;

namespace Anycubic::Plugins {
struct IStream;
struct OStream;
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
   * @brief 创建插件对话框
   * @param dlg 对话框对象
   * @param parent 父窗口指针
   * @param bmp 位图资源指针
   * @return true 创建成功
   * @return false 创建失败
   */
  virtual bool CreateDialog(wxDialog *dlg, wxWindow *parent = nullptr,
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
  virtual bool GetValue(const class wxString &, class wxString &value) = 0;

  /**
   * @brief 设置配置项
   * @param persistent 是否持久化存储，默认true
   */
  virtual bool SetValue(const class wxString &, const class wxString &value,
                        bool persistent = true) = 0;

  /**
   * @brief 获取加密配置项
   * @note 实现类应负责加解密逻辑
   */
  virtual bool GetEncryptValue(const class wxString &,
                               class wxString &value) = 0;

  /**
   * @brief 设置加密配置项
   */
  virtual bool SetEncryptValue(const class wxString &,
                               const class wxString &value,
                               bool persistent = true) = 0;

  /**
   * @brief 释放配置项值内存
   *
   * @param value 配置项值指针
   */
  virtual void Free(const char *value) = 0;
};
} // namespace Anycubic::Plugins