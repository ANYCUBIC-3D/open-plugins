#pragma once

#include <memory>
#include <webview.h>

namespace Anycubic::Plugins::SDK {

class ExecuteBase;
class Router;

struct InstanceBase {
  virtual ~InstanceBase() = default;
  virtual void Reload(void) = 0;
  virtual void RegisterApi(Router *router, ExecuteBase *executer) = 0;
};

template <typename _Ty> class Instance : public InstanceBase {
public:
  Instance() : instance_(std::make_unique<_Ty>()) {}
  explicit Instance(_Ty *instance) : instance_(instance) {}
  ~Instance() override { instance_.reset(); }
  void Reload(void) override { instance_->Reload(); }
  void RegisterApi(Router *router, ExecuteBase *executer) override {
    instance_->RegisterApi(router, executer);
  }
  _Ty *GetInstance() { return instance_.get(); }

private:
  std::unique_ptr<_Ty> instance_;
};

class WebviewApiHandler : public wxWebViewHandler {
public:
  WebviewApiHandler(const wxString &scheme, ExecuteBase *executer);
  ~WebviewApiHandler() override;

  void Reload(void);

  template <typename _Ty> void RegisterInstance() {
    instances_.push_back(new Instance<_Ty>());
    instances_.back()->RegisterApi(router_, executer_);
  }
  template <typename _Ty> void RegisterInstance(_Ty *instance) {
    instances_.push_back(new Instance<_Ty>(instance));
    instances_.back()->RegisterApi(router_, executer_);
  }

public:
  Router *operator->() { return router_; }
  Router &operator*() { return *router_; }

private:
  void StartRequest(wxWebViewHandlerRequest &request,
                    wxSharedPtr<wxWebViewHandlerResponse> response) override;

private:
  Router *router_;
  ExecuteBase *executer_;
  std::vector<InstanceBase *> instances_; ///< 实例列表
};
} // namespace Anycubic::Plugins::SDK
