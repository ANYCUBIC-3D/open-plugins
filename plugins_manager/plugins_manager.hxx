#pragma once
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <functional>
#include <memory>

class LibraryBase;
using create_library_t = std::shared_ptr<LibraryBase> (*)(void);
/**
 * @brief 插件配置管理接口
 * @note 实现类应提供持久化存储能力
 */
class PMConfig {
public:
  virtual ~PMConfig() = default;
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
};

/**
 * @brief 插件生命周期事件类型
 */
enum class EventType : int8_t {
  kEventInitByApp = 0, ///< 应用程序初始化时触发
  kEventInitByGUI,     ///< GUI框架初始化时触发
  kEventFinishedByGUI, ///< GUI初始化完成时触发
  kEventExitByGUI,     ///< GUI退出时触发
  kEventExitByApp,     ///< 应用程序退出时触发
};

/**
 * @brief 插件管理器核心接口
 */
struct PluginsManager {
  virtual ~PluginsManager() = default;
  /**
   * @brief 添加插件窗口组件
   * @param position 窗口位置标识符
   * @param widget 插件提供的窗口组件
   */
  virtual bool AddWidget(const class wxString &position,
                         class wxWindow *widget) = 0;

  /**
   * @brief 添加静态插件
   *
   * @param create  插件创建函数指针数组
   * @param count 插件数量
   * @return true 成功
   * @return false  失败
   */
  virtual bool AddStaticPlugins(create_library_t *create, size_t count) = 0;

  /**
   * @brief 设置配置管理器
   */
  virtual bool SetConfig(class PMConfig *config) = 0;

  /**
   * @brief 触发生命周期事件
   */
  virtual void EmitEvent(EventType event) = 0;

  /**
   * @brief 获取已加载插件数量
   */
  virtual size_t Plugins(void) const = 0;
};
class wxWebView;
class wxWebViewConfiguration;
extern "C" {
typedef wxWebView *(*CreateWebView_t)(
    wxWindow *parent, const wxString &url, wxWebViewConfiguration *conf,
    const std::function<void(wxWebView *)> &visitor);
/**
 * @brief 插件包元信息结构体
 */
struct PluginsPackageInfo {

  int64_t version;   ///< 插件包版本号(时间戳格式)
  time_t build_time; ///< 构建时间戳
  char md5[32];      ///< 插件包校验码
};

/**
 * @brief 获取插件包信息
 * @param plugins 插件包路径
 * @param info [输出] 插件包信息
 */
bool GetPluginsPackageInfo(const char *plugins, PluginsPackageInfo *info);

/**
 * @brief 初始化插件管理器
 * @param  plugins_dir 插件搜索目录
 * @param CreateWebView 创建webview接口
 * @param domain 主程序翻译名
 * @param tmp_dir
 * 临时解压目录，默认使用系统临时目录--当调试时，可直接指向搜索目录
 */
PluginsManager *SetupPM(const char *plugins_dir, CreateWebView_t CreateWebView,
                        const char *domain, const char *tmp_dir = nullptr);

/**
 * @brief 关闭插件管理器并释放资源
 */
void ShutdownPM(PluginsManager *pm);
}