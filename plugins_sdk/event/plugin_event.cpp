#include "plugin_event.hxx"

#include <map>

namespace Anycubic::Plugins::SDK {

class wxPluginEventPrivate {
public:
  wxPluginEventPrivate() {}
  wxPluginEventPrivate(const wxPluginEventPrivate &other) {}
  wxString GetValue(const wxString &key) const {
    if (auto itr = m_keyValueMap.find(key); itr == m_keyValueMap.end()) {
      return wxString();
    } else {
      return itr->second;
    }
  }
  void SetValue(const wxString &key, const wxString &value) {
    if (value.IsEmpty()) {
      m_keyValueMap.erase(key);
      return;
    } else {
      m_keyValueMap[key] = value;
    }
  }

private:
  std::map<wxString, wxString> m_keyValueMap;
};

wxPluginEvent::wxPluginEvent() : wxNotifyEvent(), m_private(nullptr) {}

wxPluginEvent::wxPluginEvent(wxEventType type, int id)
    : wxNotifyEvent(type, id), m_private(new wxPluginEventPrivate) {}

wxPluginEvent::wxPluginEvent(const wxPluginEvent &event)
    : wxNotifyEvent(event),
      m_private(new wxPluginEventPrivate(*event.m_private)) {}

wxPluginEvent::~wxPluginEvent(void) { delete m_private; }
wxEvent *wxPluginEvent::Clone() const { return new wxPluginEvent(*this); }
wxString wxPluginEvent::GetValue(const wxString &key) const {
  return wxString();
}
void wxPluginEvent::SetValue(const wxString &key, const wxString &value) {}

wxIMPLEMENT_DYNAMIC_CLASS(wxPluginEvent, wxNotifyEvent);
} // namespace Anycubic::Plugins::SDK
