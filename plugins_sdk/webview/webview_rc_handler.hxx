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
#include <webview.h>
#include <wx/string.h>

#include <string>
#include <unordered_map>

class wxMemoryInputStream;
namespace Anycubic::Plugins::SDK {
class WebviewRCHandler : public wxWebViewHandler {
public:
  WebviewRCHandler(const wxString &scheme, const wxString &zip,
                   const wxString &password = wxEmptyString);
  ~WebviewRCHandler() override;
  void StartRequest(wxWebViewHandlerRequest &request,
                    wxSharedPtr<wxWebViewHandlerResponse> response) override;
  wxFSFile *GetFile(const wxString &uri) override;

private:
  bool LoadPackageData();
  wxString GetFilePath(const wxString &uri) const;
  std::pair<wxMemoryInputStream *, wxString> GetStream(const wxString &path);

private:
  wxString m_zipPath;
  std::unordered_map<wxString, std::string> m_fileMap;
};
} // namespace Anycubic::Plugins::SDK
