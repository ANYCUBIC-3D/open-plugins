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
      create_webview_(CreateWebView), is_inited_(0) {
  FUNC_ENTRY;
  static bool init = false;
  if (init == false) {
    init = true;
    REGISTER_LOGGER(false);
  }
  router_ = std::make_shared<EventRouter>();
  if (!wxFileName::Exists(wxString::FromUTF8(plugins))) {
    throw std::invalid_argument("plugins not exists");
  }
  wxFileSystem::AddHandler(&fs_handler_);
  wxXmlResource::Get()->InitAllHandlers();
  wxXmlResource::Get()->AddHandler(
      new WebviewHandler(create_webview_, [this](const wxString &name) {
        return GetPlugin(name.utf8_str());
      }));
  FUNC_LEAVE;
}

PluginsManagerImpl::~PluginsManagerImpl() {
  wxFileSystem::RemoveHandler(&fs_handler_);
  wxXmlResource::Get()->ClearHandlers();
}

bool PluginsManagerImpl::AddWidget(const wxString &position, wxWindow *widget) {
  FUNC_ENTRY2("position = {}", position.utf8_string());
  if (auto itr = std::ranges::find_if(widgets_,
                                      [&position](const WidgetsNode &node) {
                                        return node.position == position;
                                      });
      itr != widgets_.end()) {
    FUNC_LEAVE;
    return false;
  }
  widgets_.emplace_back(WidgetsNode{widget, position});
  FUNC_LEAVE
  return true;
}

bool PluginsManagerImpl::AddStaticPlugins(create_library_t *create,
                                          size_t count) {
  FUNC_ENTRY2("count = {},current size: {}", count, static_plugins_.size());
  std::copy_if(create, create + count, std::back_inserter(static_plugins_),
               [](create_library_t create_library) {
                 return create_library != nullptr;
               });
  FUNC_LEAVE2("new size: {}", static_plugins_.size());
}

bool PluginsManagerImpl::SetConfig(PMConfig *config) {
  assert(config != nullptr);
  config_ = config;
  return true;
}

size_t PluginsManagerImpl::Plugins(void) const {
  FUNC_ENTRY;
  auto size = libraries_.size();
  FUNC_LEAVE2("size: {}", size);
  return size;
}

size_t PluginsManagerImpl::LoadPlugins(void) {
  FUNC_ENTRY

  // 搜索输出目录找到所有插件

  if (!::UnzipAll(plugin_packages_, tmp_dir_)) {
    FUNC_LEAVE2("Unzip plugins failed");
    return 0;
  }

  wxDir dir(wxString::FromUTF8(tmp_dir_));
  if (!dir.IsOpened()) {
    FUNC_LEAVE2("Open dir failed");
    return 0;
  }
  // 递归获取所有子目录的插件
  std::vector<wxString> plugins;
  TraverseDirectory(wxString::FromUTF8(tmp_dir_), plugins);
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
  for (auto create_library_array : static_plugins_) {
    libraries_.push_back(create_library_array());
  }

  std::ranges::transform(
      plugins, std::back_inserter(libraries_),
      [](const wxString &fname) -> std::shared_ptr<LibraryBase> {
        FUNC_ENTRY2("fname = {}", fname.utf8_string());
        if (auto lib = std::make_shared<LibraryShared>();
            lib->LoadLibrary(fname)) {
          FUNC_LEAVE;
          return lib;
        }
        LOG_ERROR("Load library {} failed", fname.utf8_string());
        FUNC_LEAVE;
        return nullptr;
      });
  decltype(libraries_) tmp_libs;
  std::swap(tmp_libs, libraries_);
  libraries_.reserve(tmp_libs.size());
  std::ranges::copy_if(tmp_libs, std::back_inserter(libraries_),
                       [](const auto &lib) {
                         if (lib == nullptr) {
                           return false;
                         }
                         auto info = lib->GetPluginInfo();
                         // NOTE: 这里决定插件会不会被加载
                         // 必需有info,且api版本为1
                         return info != nullptr && info->plugin_api == 1;
                       });
  FUNC_LEAVE2("load plugins: {}", libraries_.size());
  return libraries_.size();
}

template <typename T> static inline void delete_plugin(T *plugin) {
  if (plugin != nullptr) {
    LOG_DEBUG("Delete plugin");
    plugin->Destroy();
  }
}
bool PluginsManagerImpl::InitPlugin(std::shared_ptr<LibraryBase> lib) {
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
  if (Anycubic::Plugins::Plugin *instance = lib->SetupPlugin(this);
      instance != nullptr) {
    instances_.try_emplace(wxString::FromUTF8(instance->Name()), instance,
                           ::delete_plugin<Anycubic::Plugins::Plugin>);
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
    previous_size = 0;
    for (auto itr = libraries.begin(); itr != libraries.end();) {
      auto lib = *itr;
      if (InitPlugin(lib)) {
        libraries_.push_back(lib);
        // 删除已初始化的元素
        itr = libraries.erase(itr);
        previous_size += 1; // 标记有插件能被初始化
      } else {
        ++itr;
      }
    }

  } while (previous_size > 0 && !libraries.empty());

  LOG_ERROR_IF(!libraries.empty(), "未解决的循环依赖，剩余库: {}",
               libraries.size());
  for (const auto &[_, lib] : instances_) {
    LOG_ERROR_IF(!lib->Start(), "plugin start failed: {}", lib->Name());
  }
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

  if (auto itr = std::ranges::find_if(widgets_,
                                      [postion](const WidgetsNode &node) {
                                        return node.position == postion;
                                      });
      itr != std::end(widgets_)) {
    return itr->widget;
  }
  return nullptr;
}

bool PluginsManagerImpl::AddFS(const wxString &name, const wxString &xrc) {
  wxMemoryFSHandler::AddFile(name, xrc);
  return true;
}

bool PluginsManagerImpl::DelFS(const wxString &name) {
  wxMemoryFSHandler::RemoveFile(name);
  return true;
}

bool PluginsManagerImpl::AddFS(const wxString &name, void *data,
                               size_t length) {
  wxMemoryFSHandler::AddFile(name, data, length);
  return true;
}

bool PluginsManagerImpl::GetValue(const wxString &key, wxString &value) {
  return config_->GetValue(key, value);
}

bool PluginsManagerImpl::SetValue(const wxString &key, const wxString &value,
                                  bool persistent) {
  return config_->SetValue(key, value, persistent);
}

bool PluginsManagerImpl::GetEncryptValue(const wxString &key, wxString &value) {
  return config_->GetEncryptValue(key, value);
}

bool PluginsManagerImpl::SetEncryptValue(const wxString &key,
                                         const wxString &value,
                                         bool persistent) {
  return config_->SetEncryptValue(key, value, persistent);
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
    if (is_inited_ == 0) {
      is_inited_ = 1;
      CreateInstances();
    }
    break;
  case EventType::kEventExitByGUI:
    instances_.clear();
    widgets_.clear();
    is_inited_ = 0;
    break;
  case EventType::kEventExitByApp:
    libraries_.clear();
    break;
  default:
    break;
  }
  FUNC_LEAVE
}

static bool CheckPackage_(const std::string &plugins_) {
  FUNC_ENTRY
  std::vector<char> buffer;
  if (!::LoadSignture(plugins_.c_str(), buffer)) {
    FUNC_LEAVE2("load signture failed");
    return false;
  }

  if (!::Decode(buffer)) {
    FUNC_LEAVE2("decode failed");
    return false;
  }

  // 缓冲区有效性检查
  if (buffer.empty()) {
    FUNC_LEAVE2("empty decoded buffer");
    return false;
  }

  Package p;
  if (!::ParseInfo(buffer, &p)) {
    FUNC_LEAVE2("paser package info failed, buffer size:{}", buffer.size());
    return false;
  }

  map_type md5s;
  if (!::LoadMD5(plugins_.c_str(), md5s)) {
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

bool PluginsManagerImpl::CheckPackage() {
  FUNC_ENTRY
#ifndef NDEBUG
  FUNC_LEAVE2("check package skipped in debug mode");
  return true;
#endif // NDEBUG
  // 遍历plugins目录下的所有文件
  wxDir pluginsDir(wxString::FromUTF8(plugins_));
  if (!pluginsDir.IsOpened()) {
    FUNC_LEAVE2("plugins dir not opened:{}", plugins_);
    return false;
  }

  wxString filename;
  for (bool cont = pluginsDir.GetFirst(&filename, wxT("*.zip"), wxDIR_FILES);
       cont; cont = pluginsDir.GetNext(&filename)) {
    // 构造完整路径
    wxFileName zipFile(wxString::FromUTF8(plugins_), filename);
    if (!CheckPackage_(zipFile.GetFullPath().utf8_string())) {
      FUNC_LEAVE2("check package failed:{}", filename.utf8_string());
      return false;
    }
    plugin_packages_.push_back(filename);
  }
  FUNC_LEAVE2("check package finished, package size:{}",
              plugin_packages_.size());
  return plugin_packages_.size() > 0;
}

// 在文件末尾添加成员函数实现
void PluginsManagerImpl::TraverseDirectory(const wxString &dirPath,
                                           std::vector<wxString> &plugins) {
  wxDir dir(dirPath);
  if (!dir.IsOpened())
    return;

  wxString filename;
  // 遍历当前目录的文件和子目录
  for (bool cont = dir.GetFirst(&filename, wxT("*"), wxDIR_FILES | wxDIR_DIRS);
       cont; cont = dir.GetNext(&filename)) {
    wxFileName fullPath(filename);
    fullPath.MakeAbsolute(dirPath);

    if (wxDir::Exists(fullPath.GetFullPath())) {
      // 如果是目录且不是"."或".."，递归遍历
      if (filename != wxT(".") && filename != wxT("..")) {
        TraverseDirectory(fullPath.GetFullPath(), plugins);
      }
    } else {
      // 如果是文件且是插件，添加到列表
      if (::IsPlugins(filename)) {
        plugins.push_back(fullPath.GetFullPath());
      } else {
        LOG_INFO("{} is not plugins", filename.utf8_string());
      }
    }
  }
}