#pragma once

#include <wx/translation.h>

class acTranslationsLoader : public wxFileTranslationsLoader {
public:
  bool RegisterCatalog(const wxString &domain, const std::string &data);

public:
  virtual ~acTranslationsLoader() = default;
  wxMsgCatalog *LoadCatalog(const wxString &domain,
                            const wxString &lang) wxOVERRIDE;

private:
  using CatalogInfo = std::pair<wxString, std::string>;
  std::vector<CatalogInfo> catalogs_;
};
