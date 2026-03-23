
#include <utility/json/reader.hxx>

#include <iguana/detail/string_stream.hpp>

#include <gtest/gtest.h>

// 测试结构体
struct TestStruct {
  int id;
  std::string name;
  friend bool operator==(const TestStruct &lhs, const TestStruct &rhs) {
    return lhs.id == rhs.id && lhs.name == rhs.name;
  }
};

// 另一个测试结构体
struct NestedStruct {
  TestStruct inner;
  double value;
};

REFLECTION(TestStruct, id, name)
REFLECTION(NestedStruct, inner, value)

// 测试容器类型
TEST(PackTest, SeqContainer) {
  std::vector<TestStruct> vec = {{1, "one"}, {2, "two"}, {3, "three"}};
  iguana::string_stream ss;
  iguana::json::to_json(ss, vec);
  auto ss_str = ss.str();
  EXPECT_EQ(ss_str, "[{\"id\":1,\"name\":\"one\"},{\"id\":2,\"name\":\"two\"},{"
                    "\"id\":3,\"name\":\"three\"}]");
}
TEST(PackTest, AssContainer) {
  {
    std::map<std::string, TestStruct> map = {{"one", {1, "one"}},
                                             {"two", {2, "two"}}};
    iguana::string_stream ss;
    iguana::json::to_json(ss, map);
    auto ss_str = ss.str();
    EXPECT_EQ(ss_str, "{\"one\":{\"id\":1,\"name\":\"one\"},\"two\":{\"id\":2,"
                      "\"name\":\"two\"}}");
    decltype(map) map2;
    Anycubic::utility::json::load_from_json(map2, ss_str.data(), ss_str.size());
    EXPECT_EQ(map, map2);
  }
  {
    std::map<int, TestStruct> map = {{1, {1, "one"}}, {2, {2, "two"}}};
    iguana::string_stream ss;
    iguana::json::to_json(ss, map);
    auto ss_str = ss.str();
    EXPECT_EQ(ss_str, "{\"1\":{\"id\":1,\"name\":\"one\"},\"2\":{\"id\":2,"
                      "\"name\":\"two\"}}");
    decltype(map) map2;
    Anycubic::utility::json::load_from_json(map2, ss_str.data(), ss_str.size());
    EXPECT_EQ(map, map2);
  }
}
TEST(PackTest, TupleContainer) {
  std::tuple<std::string, int> tuple = {"one", 1};
  iguana::string_stream ss;
  iguana::json::to_json(ss, tuple);
  auto ss_str = ss.str();
  EXPECT_EQ(ss_str, "[\"one\",1]");
}