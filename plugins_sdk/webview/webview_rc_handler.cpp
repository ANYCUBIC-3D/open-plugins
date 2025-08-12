#include "webview_rc_handler.hpp"

#include <easy_log/log.hxx>
#include <resources/common.hxx>
#include <utility/codec/bin2ascii.hxx>
#include <utility/codec/md5.hxx>

#include <wx/dynlib.h>
#include <wx/filesys.h>
#include <wx/mimetype.h>
#include <wx/mstream.h>
#include <wx/uri.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#ifdef __WXMSW__
#include <Windows.h>
#define errno GetLastError()
#else
#include <sys/errno.h>
#endif //

class WebViewHandlerResponseDataMemory : public wxWebViewHandlerResponseData {
public:
  WebViewHandlerResponseDataMemory(wxMemoryInputStream *stream)
      : m_stream(stream) {}
  virtual ~WebViewHandlerResponseDataMemory() { delete m_stream; }

  virtual wxInputStream *GetStream() override { return m_stream; }

private:
  wxMemoryInputStream *m_stream;
};

WebviewRCHandler::WebviewRCHandler(const wxString &scheme,
                                   const wxString &dllPath)
    : wxWebViewHandler(scheme), m_dllPath(dllPath) {
  // 构造函数实现
#if __WXMSW__
  SetVirtualHost("localhost");
#endif

  // 加载动态库并获取包信息
  LoadPackageData();
}

WebviewRCHandler::~WebviewRCHandler() { m_fileMap.clear(); }

bool WebviewRCHandler::LoadPackageData() {
  wxDynamicLibrary dll;
  // 加载动态库
  if (!dll.Load(m_dllPath, wxDL_VERBATIM | wxDL_NOW)) {
    LOG_ERROR("Failed to load DLL: {},errno:{}", m_dllPath.utf8_string(),
              errno);
    return false;
  }

  // 获取getPackageInfo函数
  typedef PackageInfo *(*GetPackageInfoFunc)();
  GetPackageInfoFunc getPackageInfo =
      (GetPackageInfoFunc)dll.GetSymbol("getPackageInfo");

  if (!getPackageInfo) {
    LOG_ERROR("Failed to get getPackageInfo symbol from DLL");
    return false;
  }

  // 调用getPackageInfo获取包信息
  auto packageInfo = getPackageInfo();

  if (!packageInfo || !packageInfo->data || packageInfo->size == 0) {
    LOG_ERROR("Invalid package info");
    return false;
  }
  // 计算md5 保数据完整性
  char calMd5[MD5LEN] = {0};
  md5Sum(reinterpret_cast<const char *>(packageInfo->data), packageInfo->size,
         calMd5);
  char orgMD5[MD5LEN] = {0};
  hex2bin(const_cast<char *>(packageInfo->md5), orgMD5, 2 * MD5LEN);
  if (memcmp(calMd5, orgMD5, MD5LEN) != 0) {
    LOG_ERROR("md5 check failed");
    return false;
  }

  // 解析zip数据并建立文件映射
  wxMemoryInputStream memStream(
      reinterpret_cast<const void *>(packageInfo->data), packageInfo->size);
  wxZipInputStream zipStream(memStream);

  wxZipEntry *entry;
  while ((entry = zipStream.GetNextEntry()) != nullptr) {
    if (!entry->IsDir()) {
      // 读取文件内容
      size_t fileSize = entry->GetSize();
      if (fileSize > 0) {
        auto &buffer = m_fileMap[entry->GetName()];
        buffer.resize(fileSize);
        zipStream.Read(buffer.data(), fileSize);
      }
    }
    delete entry;
  }
  return true;
}

wxString WebviewRCHandler::GetFilePath(const wxString &uri) const {
  wxURI uriObj(uri);
  return uriObj.GetPath();
}

void WebviewRCHandler::StartRequest(
    wxWebViewHandlerRequest &request,
    wxSharedPtr<wxWebViewHandlerResponse> response) {

  // 如果是OPTIONS请求（预检请求），直接返回200
  if (request.GetMethod() == "OPTIONS") {
    // 设置CORS头以支持跨域请求
    response->SetHeader("Access-Control-Allow-Origin", "*");
    response->SetHeader("Access-Control-Allow-Methods",
                        "GET, POST, PUT, DELETE, OPTIONS");
    response->SetHeader("Access-Control-Allow-Headers",
                        "Content-Type, Authorization");
    response->SetStatus(200);
    response->Finish(wxString());
    return;
  }

  // 获取请求的URI
  wxString url = wxURI(request.GetURI()).GetPath();
  url = wxURI::Unescape(url);

  // 查找文件
  auto [stream, mtype] = GetStream(url);
  if (stream) {
    response->SetContentType(mtype);
    response->SetHeader("Access-Control-Allow-Origin", "*");
    response->SetHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->Finish(wxSharedPtr<wxWebViewHandlerResponseData>(
        new WebViewHandlerResponseDataMemory(stream)));
  } else {
    LOG_WARN("File not found: {}", url.utf8_string());
    response->SetStatus(404);
    response->FinishWithError();
  }
}

wxFSFile *WebviewRCHandler::GetFile(const wxString &uri) {
  wxString path = GetFilePath(uri);
  auto [stream, mimeType] = GetStream(path);
  if (stream != nullptr) {
    // 创建并返回wxFSFile
    return new wxFSFile(stream, uri, mimeType, "", wxDateTime::Now());
  }
  return nullptr;
}

std::pair<wxMemoryInputStream *, wxString>
WebviewRCHandler::GetStream(const wxString &url) {
  wxString path = url;
  if (path.StartsWith("/")) {
    path = path.Remove(0, 1);
  }
  std::pair<wxMemoryInputStream *, wxString> ret;

  auto it = m_fileMap.find(path);
  if (it != m_fileMap.end()) {
    // 创建内存流
    ret.first = new wxMemoryInputStream(it->second.data(), it->second.size());
  }

  wxString ext;
  wxFileName::SplitPath(path, nullptr, nullptr, &ext);
  static wxMimeTypesManager mime;

  if (auto m = mime.GetFileTypeFromExtension(ext); m != nullptr) {
    m->GetMimeType(&ret.second);

#ifdef __WXGTK__
  } else if (ext.CmpNoCase(wxASCII_STR("js")) == 0) {
    ret.second = wxASCII_STR("application/javascript;charset=utf-8");
#endif // __WXGTK__
  } else {
    ret.second = wxASCII_STR("application/octet-stream");
  }
  return ret;
}