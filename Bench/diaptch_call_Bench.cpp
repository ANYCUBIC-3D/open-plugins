#include <plugins_base/funcation.hxx>
#include <plugins_manager/detail/router.hpp>

#include <benchmark/benchmark.h>

#include <string>
#include <vector>

// 简单的测试数据结构 - 基于实际使用场景
struct PODData {
  int32_t a;
  float b;
  uint64_t d;
  double c;
};

struct ComplexData {
  int32_t a;
  float b;
  uint64_t d;
  double c;
  std::string e;
  std::vector<int32_t> f;
};
struct ComplexData2 {
  int32_t a;
  float b;
  uint64_t d;
  double c;
  std::string e;
  std::vector<ComplexData> f;
};
REFLECTION(ComplexData, a, b, d, c, e, f)
REFLECTION(ComplexData2, a, b, d, c, e, f)

using ComplexData3 = std::map<std::string, ComplexData2>;

bool operator==(const ComplexData &a, const ComplexData &b) {
  return a.a == b.a && a.b == b.b && a.d == b.d && a.c == b.c && a.e == b.e &&
         a.f == b.f;
}
bool operator==(const ComplexData2 &a, const ComplexData2 &b) {
  return a.a == b.a && a.b == b.b && a.d == b.d && a.c == b.c && a.e == b.e &&
         a.f == b.f;
}

constexpr const char *kPluginName = "TestPlugin";

// 测试插件类 - 专注于核心功能
class TestPlugin {
public:
  TestPlugin() {
    router.SetPluginName(kPluginName);
    RegisterFunctions(router);
  }

  void RegisterFunctions(EventRouter &router) {
    router.REGISTER_FUNCATION(TestPlugin, test_simple_function);
    router.REGISTER_FUNCATION(TestPlugin, test_pod_function);
    router.REGISTER_FUNCATION(TestPlugin, test_complex_function);
    router.REGISTER_FUNCATION(TestPlugin, test_complex_function2);
    router.REGISTER_FUNCATION(TestPlugin, test_complex_function3);
  }

public:
  inline bool ExecuteFunction(const char *plugin, const char *fname,
                              struct Anycubic::Plugins::IStream *data,
                              struct Anycubic::Plugins::OStream *result) {
    return router.ExecuteFunction(plugin, fname, data, result);
  }

private:
  int32_t test_simple_function(const char *name, int32_t a, int32_t b) {
    return a + b;
  }
  bool test_pod_function(const PODData &data) { return data.a == 10; }
  ComplexData test_complex_function(int32_t a, const ComplexData &data) {
    return data;
  }
  ComplexData2 test_complex_function2(const ComplexData2 &data) { return data; }
  ComplexData3 test_complex_function3(const ComplexData3 &data) { return data; }

private:
  EventRouter router;
};

TestPlugin g_plugin;

// ==================== 核心性能测试 ====================

// 测试1: 基本函数调用性能
static void BM_BasicFunctionCall(benchmark::State &state) {
  for (auto _ : state) {
    {
      auto result = Anycubic::Plugins::dispatch_call<int32_t>(
          &g_plugin, kPluginName, "test_simple_function", "utf8", 10, 20);
      assert(result == 30);
      benchmark::DoNotOptimize(result);
    }
    {
      PODData pod_data{10, 1.0f, 100, 1.0};
      auto result = Anycubic::Plugins::dispatch_call<bool>(
          &g_plugin, kPluginName, "test_pod_function", pod_data);
      assert(result == true);
      benchmark::DoNotOptimize(result);
    }
  }
}
BENCHMARK(BM_BasicFunctionCall);

// 测试2: 字符串处理性能
static void BM_StringProcessing(benchmark::State &state) {
  int str_length = state.range(0);
  std::string test_string(str_length, 'A');
  for (auto _ : state) {
    auto result = Anycubic::Plugins::dispatch_call<int32_t>(
        &g_plugin, kPluginName, "test_simple_function", test_string, 10, 20);
    assert(result == 30);
    benchmark::DoNotOptimize(result);
  }
  state.SetBytesProcessed(state.iterations() * test_string.length());
}
BENCHMARK(BM_StringProcessing)->Range(128, 65535);

// 测试3: 数据结构处理性能
static void BM_DataStructureProcessing(benchmark::State &state) {

  for (auto _ : state) {
    {
      ComplexData data = {10, 1.0f, 100, 1.0, "test", {1, 2, 3}};
      auto result = Anycubic::Plugins::dispatch_call<ComplexData>(
          &g_plugin, kPluginName, "test_complex_function", 10, data);
      assert(result == data);
      benchmark::DoNotOptimize(result);
    }
    {
      ComplexData2 data = {10,     1.0f,
                           100,    1.0,
                           "test", {{10, 1.0f, 100, 1.0, "test", {1, 2, 3}}}};
      auto result = Anycubic::Plugins::dispatch_call<ComplexData2>(
          &g_plugin, kPluginName, "test_complex_function2", data);
      assert(result == data);
      benchmark::DoNotOptimize(result);
    }
    {
      ComplexData3 data = {{"key1",
                            {10,
                             1.0f,
                             100,
                             1.0,
                             "test",
                             {{10, 1.0f, 100, 1.0, "test", {1, 2, 3}}}}}};
      auto result = Anycubic::Plugins::dispatch_call<ComplexData3>(
          &g_plugin, kPluginName, "test_complex_function3", data);
      assert(result == data);
      benchmark::DoNotOptimize(result);
    }
  }
}
BENCHMARK(BM_DataStructureProcessing);

// 生成大型ComplexData3数据
ComplexData3 GenerateLargeComplexData3(size_t item_count) {
  ComplexData3 data;
  for (size_t i = 0; i < item_count; ++i) {
    std::string key = "key_" + std::to_string(i);
    ComplexData2 value;
    value.a = static_cast<int32_t>(i);
    value.b = static_cast<float>(i) * 1.5f;
    value.d = static_cast<uint64_t>(i) * 1000;
    value.c = static_cast<double>(i) * 2.5;
    value.e = "string_value_" + std::to_string(i);

    // 添加嵌套数据
    for (size_t j = 0; j < 5; ++j) {
      ComplexData nested;
      nested.a = static_cast<int32_t>(j);
      nested.b = static_cast<float>(j) * 0.5f;
      nested.d = static_cast<uint64_t>(j) * 500;
      nested.c = static_cast<double>(j) * 1.5;
      nested.e = "nested_string_" + std::to_string(j);
      nested.f = {1, 2, 3, 4, 5};
      value.f.push_back(nested);
    }

    data[key] = value;
  }
  return data;
}
// 测试4: 高频调用性能
static void BM_HighFrequencyCalls(benchmark::State &state) {
  TestPlugin plugin;

  for (auto _ : state) {
    // 模拟高频调用场景
    for (int i = 0; i < 100; ++i) {
      {
        ComplexData data = {10, 1.0f, 100, 1.0, "test", {1, 2, 3}};
        auto result = Anycubic::Plugins::dispatch_call<ComplexData>(
            &g_plugin, kPluginName, "test_complex_function", 10, data);
        assert(result == data);
        benchmark::DoNotOptimize(result);
      }
      {
        ComplexData2 data = {10,     1.0f,
                             100,    1.0,
                             "test", {{10, 1.0f, 100, 1.0, "test", {1, 2, 3}}}};
        auto result = Anycubic::Plugins::dispatch_call<ComplexData2>(
            &g_plugin, kPluginName, "test_complex_function2", data);
        assert(result == data);
        benchmark::DoNotOptimize(result);
      }
      {
        ComplexData3 data = GenerateLargeComplexData3(100);
        auto result = Anycubic::Plugins::dispatch_call<ComplexData3>(
            &g_plugin, kPluginName, "test_complex_function3", data);
        assert(result == data);
        benchmark::DoNotOptimize(result);
      }
    }
  }

  state.SetItemsProcessed(state.iterations() * 100);
}
BENCHMARK(BM_HighFrequencyCalls);

// 测试5: 多线程性能
static void BM_MultiThreadedCalls(benchmark::State &state) {
  TestPlugin plugin;

  for (auto _ : state) {
    {
      ComplexData data = {10, 1.0f, 100, 1.0, "test", {1, 2, 3}};
      auto result = Anycubic::Plugins::dispatch_call<ComplexData>(
          &g_plugin, kPluginName, "test_complex_function", 10, data);
      assert(result == data);
      benchmark::DoNotOptimize(result);
    }
    {
      ComplexData2 data = {10,     1.0f,
                           100,    1.0,
                           "test", {{10, 1.0f, 100, 1.0, "test", {1, 2, 3}}}};
      auto result = Anycubic::Plugins::dispatch_call<ComplexData2>(
          &g_plugin, kPluginName, "test_complex_function2", data);
      assert(result == data);
      benchmark::DoNotOptimize(result);
    }
    {
      ComplexData3 data = GenerateLargeComplexData3(100);
      auto result = Anycubic::Plugins::dispatch_call<ComplexData3>(
          &g_plugin, kPluginName, "test_complex_function3", data);
      assert(result == data);
      benchmark::DoNotOptimize(result);
    }
  }
}

// 测试1, 2, 4, 8线程性能
BENCHMARK(BM_MultiThreadedCalls)
    ->Threads(1)
    ->Threads(2)
    ->Threads(4)
    ->Threads(8);
