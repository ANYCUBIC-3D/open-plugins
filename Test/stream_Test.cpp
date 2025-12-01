#include "plugins_base/funcation.hxx"
#include "plugins_manager/detail/router.hpp"

#include "gtest/gtest.h"

constexpr char kTestPluginName[] = "TestPlugin";
constexpr int kTestFuncationData = 42;

struct StructTestSuccess {
  const char *name;
  int v;
};
class TestPlugin {
public:
  void RegisterFuncation(EventRouter &router) {
    router.REGISTER_FUNCATION(TestPlugin, onTestFuncation);
    router.REGISTER_FUNCATION(TestPlugin, OnEmpty);
  }
  std::string onTestFuncation(const std::string &v, int *data, const char *name,
                              const StructTestSuccess &t) {
    *data = kTestFuncationData;
    return v;
  }
  std::string OnEmpty() { return ""; }
};

TEST(StreamTest, StreamTest) {
  EventRouter router;
  router.SetPluginName(kTestPluginName);
  TestPlugin plugin;
  plugin.RegisterFuncation(router);
  int data = 0;
  std::string v = "testddddddddddd";
  StructTestSuccess t = {"vvvdasdfa*", 42};

  std::string result = dispatch_call<std::string>(
      &router, kTestPluginName, "onTestFuncation", v, &data, "testchar*", t);
  EXPECT_EQ(data, kTestFuncationData);
  EXPECT_EQ(v, result);

  result = dispatch_call<std::string>(&router, kTestPluginName, "OnEmpty");
  EXPECT_TRUE(result.empty());
}
