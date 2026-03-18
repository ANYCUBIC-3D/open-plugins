// Copyright (c) 2026 深圳市纵维立方科技有限公司
// Open-Plugin is licensed under Mulan PSL v2.
// You can use this software according to the terms and conditions of the Mulan
// PSL v2.
// You may obtain a copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
// See the Mulan PSL v2 for more details.

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