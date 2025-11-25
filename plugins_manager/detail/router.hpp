#pragma once
#include "plugins_base/plugins.hxx"

#include <wx/string.h>

#include <map>
#include <memory>

class EventRouter : public Anycubic::Plugins::PluginRouter {

public:
  void SetPluginName(const wxString &name);

public:
  bool AddFunction(const char *fname,
                   Anycubic::Plugins::RequestHandler *handler) override;
  bool ExecuteFunction(const char *plugin, const char *fname,
                       struct Anycubic::Plugins::IStream *data,
                       struct Anycubic::Plugins::OStream *result) override;

public:
  virtual ~EventRouter();

private:
  wxString KeyName(const wxString &plugin, const wxString &fname) const;

private:
  wxString plugin_name_;
  std::map<wxString, std::shared_ptr<Anycubic::Plugins::RequestHandler>> funcs_;
};