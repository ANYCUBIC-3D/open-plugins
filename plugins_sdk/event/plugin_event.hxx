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
  virtual ~wxPluginEvent(void);
  wxEvent *Clone() const override;

public:
  wxString GetValue(const wxString &key) const;
  void SetValue(const wxString &key, const wxString &value);

private:
  wxPluginEventPrivate *m_private;

  wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxPluginEvent);
};

} // namespace Anycubic::Plugins::SDK
