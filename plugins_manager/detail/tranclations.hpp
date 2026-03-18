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

#include <wx/translation.h>

class acTranslationsLoader : public wxFileTranslationsLoader {
public:
  bool RegisterCatalog(const wxString &domain, const std::string &data);

public:
  virtual ~acTranslationsLoader() = default;
  wxMsgCatalog *LoadCatalog(const wxString &domain,
                            const wxString &lang) wxOVERRIDE;
  void Clear() { catalogs_.clear(); }

private:
  using CatalogInfo = std::pair<wxString, std::string>;
  std::vector<CatalogInfo> catalogs_;
};
