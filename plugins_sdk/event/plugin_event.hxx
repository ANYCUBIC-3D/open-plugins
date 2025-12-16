#pragma once
#include "event_sdk_export.hxx"

#include <wx/any.h>
#include <wx/event.h>
#include <wx/hashmap.h>

namespace Anycubic::Plugins::SDK {

class wxPluginEventPrivate;

/**
 * @brief 插件事件通用包装
 *
 */
class EVENT_API wxPluginEvent : public wxNotifyEvent {
public:
  wxPluginEvent();
  wxPluginEvent(wxEventType type, int id = wxID_ANY);
  wxPluginEvent(const wxPluginEvent &event);
  ~wxPluginEvent(void) override;
  wxEvent *Clone() const override;

  /**
   * @brief Get the Value object
   *
   * @param key 键名
   * @return wxString 值
   */
  wxString GetValue(const wxString &key) const;

  /**
   * @brief Set the Value object
   *
   * @param key 键名
   * @param value 值
   */
  void SetValue(const wxString &key, const wxString &value);

  /**
   * @brief Set the Shared Data object
   *
   * @param data 共享数据指针
   * @param ctx 上下文指针
   * @param dtor 析构函数指针
   */
  void SetSharedData(void *data, void *ctx, void (*dtor)(void *, void *));

  /**
   * @brief Get the Shared Data object
   *
   * @return void* 共享数据指针
   */
  void *GetSharedData(void) const;

  /**
   * @brief 当前事件状态
   *
   * @return true 成功
   * @return false 失败
   */
  bool IsSuccess(void) const;

  /**
   * @brief Set the Success object
   *
   * @param success 状态
   */
  void SetSuccess(bool success);

  /**
   * @brief Get the Code object
   *
   * @return int32_t 错误码
   */
  int32_t GetCode(void) const;

  /**
   * @brief Set the Code object
   *
   * @param code 错误码
   */
  void SetCode(int32_t code);

private:
  wxPluginEventPrivate *m_private;

  wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxPluginEvent);
};
using wxPluginEventFunction = void (wxEvtHandler::*)(wxPluginEvent &);

} // namespace Anycubic::Plugins::SDK
// 静态事件表支持
#define wxPluginEventHandler(func)                                             \
  wxEVENT_HANDLER_CAST(::Anycubic::Plugins::SDK::wxPluginEventFunction, func)

#define EVT_PLUGIN_EVT(evt, id, fn)                                            \
  wx__DECLARE_EVT1(evt, id, wxPluginEventHandler(fn))
