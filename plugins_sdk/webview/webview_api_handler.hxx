#pragma once

#include <memory>
#include <webview.h>

namespace Anycubic::Plugins::SDK {

class RouterBase;
class ExecuteBase;
class InstanceBase;

class WebviewApiHandler : public wxWebViewHandler {
public:
  template <typename _Ty,typename ...Args>
  _Ty* AddInstanceT(Args &&...args){
    _Ty *new_instance = new _Ty(std::forward<Args>(args)...);
    AddInstance(new_instance);
    return new_instance;
  }
  void AddInstances(const std::initializer_list<InstanceBase *> &list);
  void AddInstance(InstanceBase *instance);

public:
  WebviewApiHandler(const wxString &scheme);
  ~WebviewApiHandler(void);
  void StartRequest(wxWebViewHandlerRequest &request,
                    wxSharedPtr<wxWebViewHandlerResponse> response) override;

  void Shutdown(void);
  void Reload(void);

private:
  wxSharedPtr<RouterBase> m_router;        ///< 路由
  std::vector<InstanceBase *> m_instances; ///< 实例集
};
} // namespace Anycubic::Plugins::SDK
