#include "tranclations.hpp"

#include <easy_log/log.hxx>

wxMsgCatalog *acTranslationsLoader::LoadCatalog(const wxString &domain,
                                                const wxString &lang) {
  // 默认行为不变更，加载文件系统中的翻译文件
  if (auto cat = wxFileTranslationsLoader::LoadCatalog(domain, lang); cat) {
    return cat;
  }

  // 检查是否有注册的翻译数据
  auto it = std::ranges::find_if(catalogs_, [&domain](const CatalogInfo &info) {
    return info.first == domain;
  });
  if (it == catalogs_.end()) {
    LOG_ERROR("No catalog data registered for domain {}", domain.utf8_string());
    return nullptr;
  }
  // 判断是不是文件路径

  wxMsgCatalog *cat = nullptr;
  if (it->second.EndsWith(wxT(".mo"))) {
    cat = wxMsgCatalog::CreateFromFile(it->second, domain);
  } else {
    // 创建并加载内存中的翻译数据
    auto data = it->second.utf8_string();
    cat = wxMsgCatalog::CreateFromData(
        wxCharBuffer::CreateOwned(data.data(), data.size()), domain);
  }
  if (cat == nullptr) {
    LOG_ERROR("Failed to create catalog from data for domain {}",
              domain.utf8_string());
  }
  return cat;
}

bool acTranslationsLoader::RegisterCatalog(const wxString &domain,
                                           const wxString &path) {
  auto it = std::ranges::find_if(catalogs_, [&domain](const CatalogInfo &info) {
    return info.first == domain;
  });
  if (it != catalogs_.end()) {
    return false;
  }
  catalogs_.emplace_back(domain, path);
  return true;
}
