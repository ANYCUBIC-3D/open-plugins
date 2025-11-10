#pragma once
#include "object_base.hxx"

namespace Anycubic::Plugins::SDK {

class ScriptExecute : public ExecuteBase {
public:
  std::function<void(const wxString &)> execute;
  void ExecuteScript(const wxString &script) override { execute(script); }
  void Reload(void) override {}
  void Release(void) override { delete this; }
};
class RegexMatcher : public MatcherBase {
public:
  RegexMatcher(const wxString &pattern) : regex_(pattern) {}

  bool Match(const wxString &request) const override;
  void Reload(void) override;
  void Release(void) override;

private:
  wxString regex_;
};

class Router : public RouterBase {
public:
  bool Execute(const wxWebViewHandlerRequest &request,
               wxSharedPtr<wxWebViewHandlerResponse> response) const override;
  bool RegisterHandler(METHOD_TYPE method, const wxString &pattern,
                       HandlerBase *handler) override;

  void Reload(void) override {}
  void Release(void) override;
  static Router *Create(void);

private:
  Router(void);
  ~Router(void);

private:
  MatcherBase *make_matcher(const wxString &pattern);

private:
  struct Node {
    MatcherBase *matcher_;
    HandlerBase *handler_;
  };
  std::vector<Node> &get_handler(METHOD_TYPE method) const;
  std::vector<Node> &get_handler(METHOD_TYPE method);
  std::vector<Node> m_get_handlers;
  std::vector<Node> m_post_handlers;
  std::vector<Node> m_delete_handlers;
  std::vector<Node> m_put_handlers;
};
} // namespace Anycubic::Plugins::SDK
