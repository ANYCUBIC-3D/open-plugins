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

#include "webview_api_handler.hxx"
#include "detail/router.hpp"

#include <wx/log.h>
namespace Anycubic::Plugins::SDK {
void WebviewApiHandler::AddInstances(
    const std::initializer_list<InstanceBase *> &list) {
  std::ranges::for_each(list, std::bind(&WebviewApiHandler::AddInstance, this,
                                        std::placeholders::_1));
}

void WebviewApiHandler::AddInstance(InstanceBase *instance) {
  assert(instance != nullptr && m_router != nullptr);
  instance->Init(*m_router);
  m_instances.push_back(instance);
}

WebviewApiHandler::WebviewApiHandler(const wxString &scheme)
    : wxWebViewHandler(scheme), m_router(Router::Create()) {
  SetVirtualHost("ac.localhost");
}

WebviewApiHandler::~WebviewApiHandler() { Shutdown(); }

void WebviewApiHandler::StartRequest(
    wxWebViewHandlerRequest &request,
    wxSharedPtr<wxWebViewHandlerResponse> response) {
  if (m_router) {
    m_router->Execute(request, response);
  }
}
void WebviewApiHandler::Shutdown(void) {
  m_router.reset();
  for (auto ptr : m_instances) {
    ptr->Release();
  }
  m_instances.clear();
}

void WebviewApiHandler::Reload(void) {
  for (auto ptr : m_instances) {
    ptr->Reload();
  }
}
} // namespace Anycubic::Plugins::SDK