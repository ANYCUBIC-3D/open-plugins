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

#define SUCCESS_MASK 0x40000000
#define CODE_MASK 0x3FFFFFFF

bool wxPluginEvent::IsSuccess(void) const {
  auto val = GetInt();
  return (val & SUCCESS_MASK) != 0;
}

void wxPluginEvent::SetSuccess(bool success) {
  auto val = GetInt();
  if (success) {
    val |= SUCCESS_MASK;
  } else {
    val &= CODE_MASK;
  }
  SetInt(val);
}

int32_t wxPluginEvent::GetCode(void) const {
  auto val = GetInt();
  return val & CODE_MASK;
}

void wxPluginEvent::SetCode(int32_t code) {
  assert(code <= CODE_MASK);
  auto val = GetInt();
  val &= SUCCESS_MASK;
  val |= code;
  SetInt(val);
}

wxIMPLEMENT_DYNAMIC_CLASS(wxPluginEvent, wxNotifyEvent);
} // namespace Anycubic::Plugins::SDK
