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

#include "router.hpp"
#include <object_base.hxx>

#include <wx/regex.h>
#include <wx/uri.h>

namespace Anycubic::Plugins::SDK {

bool Router::Execute(const wxWebViewHandlerRequest &request,
                     wxSharedPtr<wxWebViewHandlerResponse> response) const {

  response->SetHeader("Access-Control-Allow-Origin", "*");
  response->SetHeader("Access-Control-Allow-Methods",
                      "GET, POST, PUT, DELETE, OPTIONS");
  response->SetHeader("Access-Control-Allow-Headers",
                      "Content-Type, Authorization");
  response->SetHeader("Access-Control-Max-Age", "3600000"); // 预检请求缓存时间
  response->SetHeader("X-Content-Type-Options", "nosniff");
  auto method = request.GetMethod();
  METHOD_TYPE m;
  if (method == "GET") {
    m = METHOD_TYPE::MethodGET;
  } else if (method == "POST") {
    m = METHOD_TYPE::MethodPOST;
  } else if (method == "PUT") {
    m = METHOD_TYPE::MethodPUT;
  } else if (method == "DELETE") {
    m = METHOD_TYPE::MethodDELETE;
  } else if (method == "OPTIONS") {
    response->SetContentType("application/json;charset=UTF-8");
    response->SetStatus(OK_200);
    wxString resBody = R"({"code":200,"msg":"Ok"})";
    response->Finish(wxSharedPtr<wxWebViewHandlerResponseData>(
        new StringResponseData(resBody)));
    return true;
  } else {
    return false;
  }

  auto &hs = get_handler(m);
  wxURI uri(request.GetRawURI());
  for (auto &h : hs) {
    if (h.matcher_->Match(uri.GetPath())) {
      return h.handler_->Execute(request, response);
    }
  }
  response->SetStatus(NotFound_404);
  response->SetContentType("application/json;charset=UTF-8");
  response->SetHeader("X-Content-Type-Options", "nosniff");
  wxString resBody = R"({"code":404,"msg":"NotFound"})";
  response->Finish(wxSharedPtr<wxWebViewHandlerResponseData>(
      new StringResponseData(resBody)));
  return false;
}

Router::Router(void) {}
Router::~Router(void) {
  for (auto m : {METHOD_TYPE::MethodGET, METHOD_TYPE::MethodPOST,
                 METHOD_TYPE::MethodDELETE, METHOD_TYPE::MethodPUT}) {
    auto &hs = get_handler(m);
    for (auto &n : hs) {
      delete n.matcher_;
      n.handler_->Release();
    }
    hs.clear();
  }
}

bool Router::RegisterHandler(METHOD_TYPE method, const wxString &pattern,
                             HandlerBase *handler) {
  auto matcher = make_matcher(pattern);
  auto &handlers = get_handler(method);
  handlers.emplace_back(Node{matcher, std::move(handler)});
  return true;
}
void Router::Release(void) { delete this; }
Router *Router::Create(void) { return new Router(); }
MatcherBase *Router::make_matcher(const wxString &pattern) {
  return new RegexMatcher(pattern);
}
const std::vector<Router::Node> &Router::get_handler(METHOD_TYPE method) const {
#define SWITCH_HANDLER(m, obj)                                                 \
  case METHOD_TYPE::m:                                                         \
    return obj;
#define BEGIN_SWITCH(m) switch (m) {
#define END_SWITCH()                                                           \
  default:                                                                     \
    assert(false);                                                             \
    }
  BEGIN_SWITCH(method)
  SWITCH_HANDLER(MethodGET, m_get_handlers)
  SWITCH_HANDLER(MethodPOST, m_post_handlers)
  SWITCH_HANDLER(MethodDELETE, m_delete_handlers)
  SWITCH_HANDLER(MethodPUT, m_put_handlers)
  END_SWITCH()
#undef SWITCH_HANDLER
#undef BEGIN_SWITCH
#undef END_SWITCH

  static std::vector<Node> tmp;
  tmp.clear();
  return tmp;
}
std::vector<Router::Node> &Router::get_handler(METHOD_TYPE method) {
#define SWITCH_HANDLER(m, obj)                                                 \
  case METHOD_TYPE::m:                                                         \
    return obj;
#define BEGIN_SWITCH(m) switch (m) {
#define END_SWITCH()                                                           \
  default:                                                                     \
    assert(false);                                                             \
    }
  BEGIN_SWITCH(method)
  SWITCH_HANDLER(MethodGET, m_get_handlers)
  SWITCH_HANDLER(MethodPOST, m_post_handlers)
  SWITCH_HANDLER(MethodDELETE, m_delete_handlers)
  SWITCH_HANDLER(MethodPUT, m_put_handlers)
  END_SWITCH()
#undef SWITCH_HANDLER
#undef BEGIN_SWITCH
#undef END_SWITCH

  static std::vector<Node> tmp;
  tmp.clear();
  return tmp;
}

bool RegexMatcher::Match(const wxString &path) const {
  return path == regex_ || wxRegEx(regex_).Matches(path);
}

void RegexMatcher::Reload(void) {}

void RegexMatcher::Release(void) { delete this; }

} // namespace Anycubic::Plugins::SDK
