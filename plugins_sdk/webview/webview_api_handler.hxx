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

#include <memory>
#include <webview.h>

namespace Anycubic::Plugins::SDK {

class RouterBase;
class ExecuteBase;
class InstanceBase;

class WebviewApiHandler : public wxWebViewHandler {
public:
  template <typename _Ty, typename... Args> _Ty *AddInstanceT(Args &&...args) {
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
