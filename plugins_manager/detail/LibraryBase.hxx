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

#pragma once

#include <plugins_base/plugins.hxx>

#include <wx/dynlib.h>

#include <type_traits>

using plugin_getInfo_t = struct PluginInfo *(*)(void);
using plugin_setup_t =
    struct Anycubic::Plugins::Plugin *(*)(struct Anycubic::Plugins::PluginHost *
                                          host);

struct LibraryBase {
  virtual ~LibraryBase() = default;
  struct PluginInfo *GetPluginInfo() const;
  struct Anycubic::Plugins::Plugin *
  SetupPlugin(struct Anycubic::Plugins::PluginHost *host) const;
  virtual bool Loaded(void) const = 0;
  virtual bool LoadLibrary(const wxString &libName) = 0;
  virtual void UnloadLibrary() = 0;

protected:
  plugin_getInfo_t getInfo_;
  plugin_setup_t setup_;
};

struct LibraryShared : public LibraryBase {
public:
  LibraryShared();
  ~LibraryShared() override;

public:
  inline bool Loaded(void) const override { return m_library.IsLoaded(); }
  bool LoadLibrary(const wxString &libName) override;
  void UnloadLibrary() override;

private:
  template <typename _Ty>
  bool GetProcAddress(const wxString &proc, _Ty &ptr) const {
    ptr = reinterpret_cast<_Ty>(m_library.RawGetSymbol(proc));
    return ptr != nullptr;
  }
  bool GetProcAddressAll(const wxString &pluginName);

private:
  LibraryShared(const LibraryShared &) = delete;
  LibraryShared &operator=(const LibraryShared &) = delete;

private:
  wxDynamicLibrary m_library;
};

struct LibraryStatic : public LibraryBase {
public:
  LibraryStatic(plugin_getInfo_t getInfo, plugin_setup_t setup);
  ~LibraryStatic() override;

  bool LoadLibrary(const wxString &libName) override;
  void UnloadLibrary() override;
  inline bool Loaded(void) const override { return true; }
};
