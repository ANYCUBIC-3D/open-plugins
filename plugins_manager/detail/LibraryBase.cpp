#include "LibraryBase.hxx"
#include <assert.h>

PluginInfo *LibraryBase::GetPluginInfo() const {
  assert(Loaded());
  if (getInfo_ == nullptr)
    return nullptr;
  return getInfo_();
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

bool LibraryShared::LoadLibrary(wxString libName) {
  if (Loaded())
    return true;
  auto pluginName = GetPluginName(libName);
  if (pluginName.IsEmpty()) {
    return false;
  }
  wxDynamicLibrary loaderDll;
  if (!loaderDll.Load(libName, wxDL_DEFAULT | wxDL_QUIET))
    return false;
  m_library.Attach(loaderDll.Detach());
  return GetProcAddressAll(pluginName);
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

bool LibraryStatic::LoadLibrary(wxString libName) { return true; }

void LibraryStatic::UnloadLibrary() {}
