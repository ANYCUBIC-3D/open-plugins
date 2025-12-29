#include "router.hpp"

#include <easy_log/stackstrace.hxx>

#include <assert.h>

void global_destroy(Anycubic::Plugins::RequestHandler *p) {
  FUNC_ENTRY;
  assert(p != nullptr);
  p->Destroy();
  FUNC_LEAVE;
}
void EventRouter::SetPluginName(const wxString &name) {
  FUNC_ENTRY2("plugin_name_ = {}", name.utf8_string());
  plugin_name_ = name;
  FUNC_LEAVE;
}

bool EventRouter::AddFunction(const char *fname,
                              Anycubic::Plugins::RequestHandler *handler) {
  FUNC_ENTRY2("fname = {}, plugin_name_ = {}", fname,
              plugin_name_.utf8_string());
  assert(!plugin_name_.IsEmpty());
  auto key = KeyName(plugin_name_, wxASCII_STR(fname));
  if (funcs_.contains(key)) {
    FUNC_LEAVE2("function {} already added", fname);
    return false;
  }
  auto result =
      funcs_.emplace(key, std::shared_ptr<Anycubic::Plugins::RequestHandler>(
                              handler, global_destroy));
  assert(result.second);
  FUNC_LEAVE2("function {} added: {}", fname, result.second);
  return result.second;
}

bool EventRouter::ExecuteFunction(const char *plugin, const char *fname,
                                  Anycubic::Plugins::IStream *data,
                                  Anycubic::Plugins::OStream *result) {
  FUNC_ENTRY2("plugin = {}, fname = {}", plugin, fname);
  auto key = KeyName(wxASCII_STR(plugin), wxASCII_STR(fname));
  if (auto itr = funcs_.find(key); itr != funcs_.end()) {
    try {
      itr->second->Execute(data, result);
    } catch (const std::exception &e) {
      FUNC_LEAVE2("function {} execute failed: {}", fname, e.what());
      return false;
    }
    FUNC_LEAVE;
    return true;
  }
  assert(false);
  FUNC_LEAVE2("function {} not found", fname);
  return false;
}

EventRouter::~EventRouter() { funcs_.clear(); }

wxString EventRouter::KeyName(const wxString &plugin,
                              const wxString &fname) const {
  FUNC_ENTRY2("plugin = {}, fname = {}", plugin.utf8_string(),
              fname.utf8_string());
  assert(!plugin.IsEmpty());
  assert(!fname.IsEmpty());
  FUNC_LEAVE2("key = {}",
              wxString::Format("%s/%s", plugin, fname).utf8_string());
  return wxString::Format("%s/%s", plugin, fname);
}
