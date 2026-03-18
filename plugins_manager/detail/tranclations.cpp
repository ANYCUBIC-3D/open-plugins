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

#include "tranclations.hpp"

#include <easy_log/log.hxx>

#include <boost/algorithm/string.hpp>

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
  if (boost::algorithm::ends_with(it->second, ".mo")) {
    cat = wxMsgCatalog::CreateFromFile(it->second, domain);
  } else {
    // 创建并加载内存中的翻译数据
    auto buffer =
        wxCharBuffer::CreateOwned(it->second.data(), it->second.size());
    cat = wxMsgCatalog::CreateFromData(buffer, domain);
  }
  if (cat == nullptr) {
    LOG_ERROR("Failed to create catalog from data for domain {}",
              domain.utf8_string());
  }
  return cat;
}

bool acTranslationsLoader::RegisterCatalog(const wxString &domain,
                                           const std::string &path) {
  auto it = std::ranges::find_if(catalogs_, [&domain](const CatalogInfo &info) {
    return info.first == domain;
  });
  if (it != catalogs_.end()) {
    return false;
  }
  catalogs_.emplace_back(domain, path);
  return true;
}
