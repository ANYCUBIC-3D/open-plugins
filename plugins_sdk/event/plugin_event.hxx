#pragma once
#include "event_sdk_export.hxx"

#include <wx/any.h>
#include <wx/event.h>
#include <wx/hashmap.h>

namespace Anycubic::Plugins::SDK {

class wxPluginEventPrivate;
class EVENT_API wxPluginEvent : public wxNotifyEvent {
public:
  wxPluginEvent();
  wxPluginEvent(wxEventType type, int id);
  wxPluginEvent(const wxPluginEvent &event);
  ~wxPluginEvent(void) override;
  wxEvent *Clone() const override;

  wxString GetValue(const wxString &key) const;
  void SetValue(const wxString &key, const wxString &value);
  bool IsSuccess(void) const;
  void SetSuccess(bool success);
  int32_t GetCode(void) const;
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
