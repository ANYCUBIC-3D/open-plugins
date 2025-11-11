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
