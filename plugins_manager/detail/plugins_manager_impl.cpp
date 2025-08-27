#include "plugins_manager_impl.hpp"
#include "LibraryBase.hxx"
#include "package.hxx"
#include "xrc/xh_webview.hpp"

#include <plugins_base/plugins_base.hxx>

#include <easy_log/stackstrace.hxx>

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/xrc/xmlres.h>
#if PLUGINS_LIST_SIZE > 0
create_library_t *GetCreateLibraryArray();
#endif
PluginsManagerImpl::PluginsManagerImpl(const char *plugins, const char *tmp_dir,
                                       CreateWebView_t CreateWebView)
    : plugins_(plugins), tmp_dir_(tmp_dir), config_(nullptr),
      create_webview_(CreateWebView) {
  static bool init = false;
  if (init == false) {
    init = true;
    REGISTER_LOGGER(false);
  }
  if (!wxFileName::Exists(wxString::FromUTF8(plugins))) {
    throw std::invalid_argument("plugins not exists");
  }
  wxFileSystem::AddHandler(&fs_handler_);
  wxXmlResource::Get()->InitAllHandlers();
  wxXmlResource::Get()->AddHandler(
      new WebviewHandler(create_webview_, [this](const wxString &name) {
        return GetPlugin(name.utf8_str());
      }));
}

PluginsManagerImpl::~PluginsManagerImpl() {
  wxFileSystem::RemoveHandler(&fs_handler_);
  wxXmlResource::Get()->ClearHandlers();
}

bool PluginsManagerImpl::AddWidget(const wxString &position, wxWindow *widget) {
  FUNC_ENTRY
  auto itr = std::find_if(widgets_.begin(), widgets_.end(),
                          [position](const WidgetsNode &node) {
                            return node.position == position;
                          });
  if (itr != widgets_.end()) {
    FUNC_LEAVE2("{} widgets exists", position.utf8_string());
    return false;
  }
  widgets_.emplace_back(widget, position);
  FUNC_LEAVE
  return true;
}

bool PluginsManagerImpl::AddStaticPlugins(create_library_t *create,
                                          size_t count) {
  for (size_t idx = 0; idx < count; idx++) {
    if (create[idx] == nullptr) {
      continue;
    }
    static_plugins_.push_back(create[idx]);
  }
  return true;
}

bool PluginsManagerImpl::SetConfig(PMConfig *config) {
  config_ = config;
  return true;
}

size_t PluginsManagerImpl::Plugins(void) const { return libraries_.size(); }

size_t PluginsManagerImpl::LoadPlugins(void) {
  FUNC_ENTRY

  // 搜索输出目录找到所有插件
  std::vector<wxString> plugins;
#ifndef NDEBUG
  auto env = std::getenv("PLUGINS_DEBUG_DIR");
  if (env == nullptr) {
#endif // NDEBUG
    if (!Unzip(plugins_, tmp_dir_)) {
      FUNC_LEAVE2("Unzip plugins failed");
      return 0;
    }
#ifndef NDEBUG
  } else {
    tmp_dir_ = env;
  }
#endif // NDEBUG

  wxDir dir(wxString::FromUTF8(tmp_dir_));
  if (!dir.IsOpened()) {
    FUNC_LEAVE2("Open dir failed");
    return 0;
  }
  wxString filename;
  for (bool cont = dir.GetFirst(&filename, wxT("*"), wxDIR_FILES); cont;
       cont = dir.GetNext(&filename)) {
    if (!IsPlugins(filename)) {
      LOG_INFO("{} is not plugins", filename.utf8_string());
      continue;
    }
    plugins.push_back(filename);
  }
  libraries_.reserve(plugins.size() + PLUGINS_LIST_SIZE +
                     static_plugins_.size());
#if PLUGINS_LIST_SIZE > 0
  auto create_library_array = GetCreateLibraryArray();
  for (int idx = 0; idx < PLUGINS_LIST_SIZE; idx++) {
    if (create_library_array[idx] != nullptr) {
      libraries_.push_back(create_library_array[idx]());
    }
  }
#endif
  for (int idx = 0; idx < static_plugins_.size(); idx++) {
    libraries_.push_back(static_plugins_[idx]());
  }
  std::transform(plugins.begin(), plugins.end(), std::back_inserter(libraries_),
                 [](const wxString &filename) -> std::shared_ptr<LibraryBase> {
                   auto lib = std::shared_ptr<LibraryBase>(new LibraryShared());
                   if (lib->LoadLibrary(filename)) {
                     return lib;
                   }
                   LOG_ERROR("Load library {} failed", filename.utf8_string());
                   return nullptr;
                 });
  std::erase_if(libraries_, [](const auto &lib) {
    if (lib == nullptr) {
      return true;
    }
    auto info = lib->GetPluginInfo();
    // NOTE: 这里决定插件会不会被加载
    // 必需有info,且api版本为1
    return info == nullptr || info->plugin_api != 1;
  });

  return libraries_.size();
}

template <typename T> static inline void delete_plugin(T *plugin) {
  if (plugin != nullptr) {
    LOG_DEBUG("Delete plugin");
    plugin->Destroy();
  }
}
bool PluginsManagerImpl::InitPlugin(std::shared_ptr<LibraryBase> &lib) {
  FUNC_ENTRY
  auto info = lib->GetPluginInfo();
  if (info == nullptr) {
    FUNC_LEAVE2("Get plugin info failed");
    return false;
  }
  for (auto idx = 0; idx < info->dependency_count; idx++) {
    if (!HasPlugin(info->dependency[idx])) {
      FUNC_LEAVE2("plugin {} dependency {} not found", info->name,
                  info->dependency[idx]);
      return false;
    }
  }
  router_->SetPluginName(info->name);
  if (auto instance = lib->SetupPlugin(this)) {
    instances_.emplace(wxString::FromUTF8(info->name),
                       std::shared_ptr<Anycubic::Plugins::Plugin>(
                           instance, delete_plugin<Anycubic::Plugins::Plugin>));
    LOG_INFO("Init plugin {} success", info->name);
  } else {
    LOG_ERROR("Setup plugin {} failed", info->name);
  }

  FUNC_LEAVE
  // 让上层清理lib
  return true;
}
bool PluginsManagerImpl::CreateInstances(void) {
  FUNC_ENTRY
  size_t previous_size = 0;
  std::vector<std::shared_ptr<LibraryBase>> libraries;
  std::swap(libraries_, libraries);
  do {
    previous_size = std::erase_if(libraries, [this](auto &lib) {
      if (InitPlugin(lib)) {
        libraries_.push_back(lib);
        return true; // 删除已初始化的元素
      }
      return false;
    });
  } while (previous_size > 0 && !libraries.empty());

  LOG_ERROR_IF(!libraries.empty(), "未解决的循环依赖，剩余库: {}",
               libraries.size());
  FUNC_LEAVE
  return !instances_.empty();
}

Anycubic::Plugins::PluginRouter *PluginsManagerImpl::Router(void) {
  return router_.get();
}

bool PluginsManagerImpl::HasPlugin(const char *name) {
  return instances_.contains(wxString::FromUTF8(name));
}

Anycubic::Plugins::Plugin *PluginsManagerImpl::GetPlugin(const char *name) {
  auto itr = instances_.find(wxString::FromUTF8(name));
  if (itr == instances_.end()) {
    return nullptr;
  }
  return itr->second.get();
}

wxWindow *PluginsManagerImpl::GetWindow(const char *postion) {
  auto itr = std::find_if(
      widgets_.begin(), widgets_.end(),
      [postion](const WidgetsNode &node) { return node.position == postion; });
  if (itr != std::end(widgets_)) {
    return itr->widget;
  }
  return nullptr;
}

bool PluginsManagerImpl::AddFS(const wxString &name, const wxString &xrc) {
  fs_handler_.AddFile(name, xrc);
  return true;
}

bool PluginsManagerImpl::DelFS(const wxString &name) {
  fs_handler_.RemoveFile(name);
  return true;
}

bool PluginsManagerImpl::AddFS(const wxString &name, void *data,
                               size_t length) {
  fs_handler_.AddFile(name, data, length);
  return true;
}

bool PluginsManagerImpl::SaveConfig(const char *name, const char *value) {
  assert(config_ != nullptr);
  return config_->SetValue(wxString::FromUTF8(name), wxString::FromUTF8(value));
}

bool PluginsManagerImpl::LoadConfig(const char *name, const char **value) {
  assert(config_ != nullptr && value != nullptr);
  wxString value_;
  if (config_->GetValue(wxString::FromUTF8(name), value_)) {
    *value = ::strdup(value_.utf8_str());
    return true;
  }
  return false;
}

void PluginsManagerImpl::Free(const char *value) {
  ::free(const_cast<char *>(value));
}

void PluginsManagerImpl::EmitEvent(EventType event) {
  FUNC_ENTRY2("{}", static_cast<int32_t>(event));
  assert(wxIsMainThread());
  switch (event) {
  case EventType::kEventInitByApp:
    LoadPlugins();
    break;
  case EventType::kEventInitByGUI:
    // empty
    break;
  case EventType::kEventFinishedByGUI:
    CreateInstances();
    break;
  case EventType::kEventExitByGUI:
    instances_.clear();
    widgets_.clear();
    break;
  case EventType::kEventExitByApp:
    libraries_.clear();
    break;
  default:
    break;
  }
  FUNC_LEAVE
}

bool PluginsManagerImpl::CheckPackage() {
  FUNC_ENTRY
  std::vector<char> buffer;
  if (!LoadSignture(plugins_.c_str(), buffer)) {
    FUNC_LEAVE2("load signture failed");
    return false;
  }

  if (!Decode(buffer)) {
    FUNC_LEAVE2("decode failed");
    return false;
  }

  // 缓冲区有效性检查
  if (buffer.empty()) {
    FUNC_LEAVE2("empty decoded buffer");
    return false;
  }

  Package p;
  if (!ParseInfo(buffer, &p)) {
    FUNC_LEAVE2("paser package info failed, buffer size:{}", buffer.size());
    return false;
  }

  std::map<std::string, std::string> md5s;
  if (!LoadMD5(plugins_.c_str(), md5s)) {
    FUNC_LEAVE2("load MD5 failed for package:{}", plugins_);
    return false;
  }

  for (auto [key, value] : md5s) {
    if (auto itr = p.files.find(key); itr == p.files.end()) {
      FUNC_LEAVE2("filename not found:{}", key);
      return false;
    } else if (itr->second != value) {
      FUNC_LEAVE2("md5 not match,s:{} c:{}", itr->second, value);
      return false;
    }
  }
  FUNC_LEAVE
  return true;
}
