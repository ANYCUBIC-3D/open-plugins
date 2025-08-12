#pragma once

#include <webview.h>

#include <string>
#include <unordered_map>

class WebviewRCHandler : public wxWebViewHandler {
public:
  WebviewRCHandler(const wxString &scheme, const wxString &dllPath);
  ~WebviewRCHandler();
  void StartRequest(wxWebViewHandlerRequest &request,
                    wxSharedPtr<wxWebViewHandlerResponse> response) override;
  wxFSFile *GetFile(const wxString &uri) override;

private:
  bool LoadPackageData();
  wxString GetFilePath(const wxString &uri) const;
  std::pair<class wxMemoryInputStream *, wxString>
  GetStream(const wxString &path);

private:
  wxString m_dllPath;
  std::unordered_map<wxString, std::string> m_fileMap;
};