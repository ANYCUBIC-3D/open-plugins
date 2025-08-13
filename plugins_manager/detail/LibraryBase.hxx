#pragma once
#include "package.hpp"

#include <plugins_base/plugins.hxx>

#include <wx/dynlib.h>

#include <type_traits>

typedef struct PluginInfo *(*plugin_getInfo_t)(void);
typedef struct Anycubic::Plugins::Plugin *(*plugin_setup_t)(
    struct Anycubic::Plugins::PluginHost *host);

struct LibraryBase {
  struct PluginInfo *GetPluginInfo() const;
  struct Anycubic::Plugins::Plugin *
  SetupPlugin(struct Anycubic::Plugins::PluginHost *host) const;
  virtual bool Loaded(void) const = 0;
  virtual bool LoadLibrary(wxString libName) = 0;
  virtual void UnloadLibrary() = 0;

protected:
  plugin_getInfo_t getInfo_;
  plugin_setup_t setup_;
};

struct LibraryShared : public LibraryBase {
public:
  LibraryShared();
  virtual ~LibraryShared();

public:
  inline bool Loaded(void) const override { return m_library.IsLoaded(); }
  bool LoadLibrary(wxString libName) override;
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
  virtual ~LibraryStatic();

  bool LoadLibrary(wxString libName) override;
  void UnloadLibrary() override;
  inline bool Loaded(void) const override { return true; }
};

typedef std::shared_ptr<LibraryBase> (*create_library_t)(void);