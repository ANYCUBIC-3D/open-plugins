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
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include < Windows.h>
#endif

#include "LibraryBase.hxx"
#include "package.hxx"

#include <easy_log/stackstrace.hxx>

#include <assert.h>

#include <wx/filename.h>

PluginInfo *LibraryBase::GetPluginInfo() const {
  FUNC_ENTRY;
  assert(Loaded());
  if (getInfo_ == nullptr) {
    FUNC_LEAVE;
    return nullptr;
  }
  auto info = getInfo_();
  FUNC_LEAVE;
  return info;
}

Anycubic::Plugins::Plugin *
LibraryBase::SetupPlugin(Anycubic::Plugins::PluginHost *host) const {
  assert(Loaded());
  if (setup_ == nullptr)
    return nullptr;
  return setup_(host);
}

LibraryShared::LibraryShared() {
  getInfo_ = nullptr;
  setup_ = nullptr;
}
LibraryShared::~LibraryShared() { UnloadLibrary(); }

bool LibraryShared::LoadLibrary(const wxString &libName) {
  FUNC_ENTRY2("libName = {}", libName.utf8_string());
  if (Loaded()) {
    FUNC_LEAVE
    return true;
  }
  auto pluginName = GetPluginName(libName);
  if (pluginName.IsEmpty()) {
    FUNC_LEAVE
    return false;
  }
#if defined(__WXMSW__)
  auto filename = wxFileName::FileName(libName);
  SetDllDirectoryW(filename.GetPath().ToStdWstring().c_str());
#endif // __WXMSW__
  wxDynamicLibrary loaderDll;
  if (!loaderDll.Load(libName, wxDL_DEFAULT | wxDL_QUIET)) {
#if defined(__WXMSW__)
    FUNC_LEAVE2("Failed to load shared library error code: {}", GetLastError());
#else
    FUNC_LEAVE2("Failed to load shared library error code: {}", errno);
#endif

    return false;
  }
  m_library.Attach(loaderDll.Detach());
  auto result = GetProcAddressAll(pluginName);
  FUNC_LEAVE2("result = {}", result);
  return result;
}
void LibraryShared::UnloadLibrary() {
  getInfo_ = nullptr;
  setup_ = nullptr;
  m_library.Unload();
}

bool LibraryShared::GetProcAddressAll(const wxString &pluginName) {
  auto getInfoName = wxString::Format("%s_getInfo", pluginName);
  auto setupName = wxString::Format("%s_setup", pluginName);
  return GetProcAddress(getInfoName, getInfo_) &&
         GetProcAddress(setupName, setup_);
}

LibraryStatic::LibraryStatic(plugin_getInfo_t getInfo, plugin_setup_t setup) {
  getInfo_ = getInfo;
  setup_ = setup;
  assert(getInfo != nullptr && setup != nullptr);
}
LibraryStatic::~LibraryStatic() {}

bool LibraryStatic::LoadLibrary(const wxString &libName) { return true; }

void LibraryStatic::UnloadLibrary() {}
