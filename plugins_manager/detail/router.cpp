#include "router.hpp"

#include <assert.h>

void EventRouter::SetPluginName(const wxString &name) { plugin_name_ = name; }

bool EventRouter::AddFunction(const char *fname,
                              Anycubic::Plugins::RequestHandler *handler) {
  assert(!plugin_name_.IsEmpty());
  auto key = KeyName(plugin_name_, wxASCII_STR(fname));
  if (funcs_.contains(key)) {
    return false;
  }
  funcs_[key] = std::shared_ptr<Anycubic::Plugins::RequestHandler>(
      handler, [](Anycubic::Plugins::RequestHandler *p) { p->Destroy(); });
  return true;
}

bool EventRouter::ExecuteFunction(const char *plugin, const char *fname,
                                  Anycubic::Plugins::IStream *data,
                                  Anycubic::Plugins::OStream *result) {
  auto key = KeyName(wxASCII_STR(plugin), wxASCII_STR(fname));
  if (funcs_.contains(key)) {
    return false;
  }
  funcs_[key]->Execute(data, result);
  return true;
}

wxString EventRouter::KeyName(const wxString &plugin,
                              const wxString &fname) const {
  assert(!plugin.IsEmpty());
  assert(!fname.IsEmpty());
  return wxString::Format("%s/%s", plugin, fname);
}
