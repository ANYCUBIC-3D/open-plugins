#include "router.hxx"

#include <wx/regex.h>
namespace Anycubic::Plugins::SDK {
bool Router::dispatch(wxWebViewHandlerRequest &request,
                      wxSharedPtr<wxWebViewHandlerResponse> response) {
  response->SetHeader("Access-Control-Allow-Origin", "*");
  response->SetHeader("Access-Control-Allow-Methods",
                      "GET, POST, PUT, DELETE, OPTIONS");
  response->SetHeader("Access-Control-Allow-Headers",
                      "Content-Type, Authorization");
  response->SetHeader("Access-Control-Max-Age", "3600000"); // 预检请求缓存时间
  response->SetHeader("X-Content-Type-Options", "nosniff");
  auto method = request.GetMethod();
  METHOD m;
  if (method == "GET") {
    m = METHOD::MethodGET;
  } else if (method == "POST") {
    m = METHOD::MethodPOST;
  } else if (method == "PUT") {
    m = METHOD::MethodPUT;
  } else if (method == "DELETE") {
    m = METHOD::MethodDELETE;
  } else if (method == "OPTIONS") {
    response->SetContentType("application/json;charset=UTF-8");
    response->SetStatus(OK_200);
    wxString resBody = R"({"code":200,"msg":"Ok"})";
    response->Finish(wxSharedPtr<wxWebViewHandlerResponseData>(
        new handlerStringResponseData(resBody)));
    return true;
  } else {
    return false;
  }

  auto &hs = get_handler(m);
  wxURI uri(request.GetRawURI());
  for (auto &h : hs) {
    if (h.matcher_->match(uri.GetPath())) {
      return h.handler_(request, response);
    }
  }
  response->SetStatus(NotFound_404);
  response->SetContentType("application/json;charset=UTF-8");
  response->SetHeader("X-Content-Type-Options", "nosniff");
  wxString resBody = R"({"code":404,"msg":"NotFound"})";
  response->Finish(wxSharedPtr<wxWebViewHandlerResponseData>(
      new handlerStringResponseData(resBody)));
  return false;
}

Router::Router(void) {}
Router::~Router(void) {
  for (auto m : {METHOD::MethodGET, METHOD::MethodPOST, METHOD::MethodDELETE,
                 METHOD::MethodPUT}) {
    auto hs = get_handler(m);
    for (auto &n : hs) {
      delete n.matcher_;
    }
    hs.clear();
  }
}

bool Router::add_handler(METHOD method, const wxString &pattern,
                         Handler &&handler) {
  auto matcher = make_matcher(pattern);
  auto &handlers = get_handler(method);
  handlers.emplace_back(Node{matcher, std::move(handler)});
  return true;
}
MatcherBase *Router::make_matcher(const wxString &pattern) {
  return new RegexMatcher(pattern);
}

std::vector<Router::Node> &Router::get_handler(METHOD method) {
  switch (method) {
  case METHOD::MethodGET:
    return m_get_handlers;
  case METHOD::MethodPOST:
    return m_post_handlers;
  case METHOD::MethodDELETE:
    return m_delete_handlers;
  case METHOD::MethodPUT:
    return m_put_handlers;
  default: {
    assert(false);
    static std::vector<Node> tmp;
    return tmp;
  }
  }
}
bool RegexMatcher::match(const wxString &path) const {
  return path == regex_ || wxRegEx(regex_).Matches(path);
}

handlerStringResponseData::handlerStringResponseData(wxString &data)
    : m_stream(data) {}
wxInputStream *handlerStringResponseData::GetStream() { return &m_stream; }
} // namespace Anycubic::Plugins::SDK
