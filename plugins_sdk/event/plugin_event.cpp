// Copyright (c) [Year] [name of copyright holder]
// Open-Plugin is licensed under Mulan PSL v2.
// You can use this software according to the terms and conditions of the Mulan
// PSL v2.
// You may obtain a copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
// See the Mulan PSL v2 for more details.

#include "plugin_event.hxx"

#include <map>
#include <memory>

namespace Anycubic::Plugins::SDK {

class wxPluginEventPrivate {
public:
  wxPluginEventPrivate() = default;
  wxPluginEventPrivate(const wxPluginEventPrivate &other) = default;
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
  std::shared_ptr<void> &SharedData() { return m_sharedData; }

private:
  std::map<wxString, wxString> m_keyValueMap;
  std::shared_ptr<void> m_sharedData;
};

wxPluginEvent::wxPluginEvent() : wxPluginEvent(wxEVT_NULL, wxID_ANY) {}

wxPluginEvent::wxPluginEvent(wxEventType type, int id)
    : wxNotifyEvent(type, id), m_private(new wxPluginEventPrivate) {}

wxPluginEvent::wxPluginEvent(const wxPluginEvent &event)
    : wxNotifyEvent(event),
      m_private(new wxPluginEventPrivate(*event.m_private)) {}

wxPluginEvent::~wxPluginEvent(void) { delete m_private; }
wxEvent *wxPluginEvent::Clone() const { return new wxPluginEvent(*this); }
wxString wxPluginEvent::GetValue(const wxString &key) const {
  return m_private->GetValue(key);
}
void wxPluginEvent::SetValue(const wxString &key, const wxString &value) {
  m_private->SetValue(key, value);
}

void wxPluginEvent::SetSharedData(void *data, void *ctx,
                                  void (*dtor)(void *, void *)) {
  m_private->SharedData() =
      std::shared_ptr<void>(data, [ctx, dtor](void *data) { dtor(data, ctx); });
}

void *wxPluginEvent::GetSharedData(void) const {
  return m_private->SharedData().get();
}

constexpr int SUCCESS_MASK = 0x40000000;
constexpr int CODE_MASK = 0x3FFFFFFF;

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
