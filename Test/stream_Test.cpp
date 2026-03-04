#include "plugins_base/funcation.hxx"
#include "plugins_manager/detail/router.hpp"

#include "gtest/gtest.h"

// 测试结构体
struct TestStruct {
  int id;
  std::string name;
};

// 另一个测试结构体
struct NestedStruct {
  TestStruct inner;
  double value;
};

REFLECTION(TestStruct, id, name)
REFLECTION(NestedStruct, inner, value)

// 测试 get_bytes 函数
TEST(PackTest, GetTypeSizeArithmetic) {
  int i = 42;
  double d = 3.14;
  char c = 'a';

  EXPECT_EQ(Anycubic::Plugins::get_bytes(i), sizeof(int));
  EXPECT_EQ(Anycubic::Plugins::get_bytes(d), sizeof(double));
  EXPECT_EQ(Anycubic::Plugins::get_bytes(c), sizeof(char));
}

TEST(PackTest, GetTypeSizeString) {
  std::string str = "hello world";
  wxString wxStr("hello wxWidgets");
  const char *cstr = "hello c string";

  // std::string: length + sizeof(uint16_t)
  EXPECT_EQ(Anycubic::Plugins::get_bytes(str), str.length() + sizeof(uint16_t));

  // wxString: converted to utf8 string and recursively called
  size_t wxSize = Anycubic::Plugins::get_bytes(wxStr);
  EXPECT_EQ(wxSize, wxStr.utf8_string().length() + sizeof(uint16_t));

  // C string: length + sizeof(uint16_t)
  EXPECT_EQ(Anycubic::Plugins::get_bytes(cstr),
            strlen(cstr) + sizeof(uint16_t));
}

TEST(PackTest, GetTypeSizeNullCString) {
  const char *nullStr = nullptr;
  EXPECT_EQ(Anycubic::Plugins::get_bytes(nullStr), sizeof(uint16_t));
}

TEST(PackTest, GetTypeSizePOD) {
  struct SimpleStruct {
    int a;
    float b;
  } pod = {42, 3.14f};

  EXPECT_EQ(Anycubic::Plugins::get_bytes(pod), sizeof(SimpleStruct));
}

TEST(PackTest, GetTypeSizeJsonStruct) {
  TestStruct ts{1, "test"};
  size_t size = Anycubic::Plugins::get_bytes(ts);
  // The size should be based on the JSON representation
  EXPECT_GT(size, sizeof(uint16_t));
}

TEST(PackTest, GetTypeSizeNestedJsonStruct) {
  TestStruct inner{1, "inner"};
  NestedStruct ns{inner, 3.14};
  size_t size = Anycubic::Plugins::get_bytes(ns);
  EXPECT_GT(size, 0);
}

TEST(PackTest, GetBytesVariadic) {
  int i = 42;
  std::string str = "hello";
  char c = 'x';

  size_t total = Anycubic::Plugins::get_bytes(i, str, c);
  size_t expected =
      sizeof(int) + (str.length() + sizeof(uint16_t)) + sizeof(char);
  EXPECT_EQ(total, expected);
}

// 测试OStream和IStream功能
TEST(PackTest, OStreamBasic) {
  int val = 123;
  size_t size = Anycubic::Plugins::get_bytes(val);

  Anycubic::Plugins::OStream os(size);

  bool result = os.Write(val);
  EXPECT_TRUE(result);

  EXPECT_EQ(os.Size(), sizeof(int));
}

TEST(PackTest, IStreamBasic) {
  int original = 123;
  size_t size = Anycubic::Plugins::get_bytes(original);

  Anycubic::Plugins::OStream os(size);
  os.Write(original);

  Anycubic::Plugins::IStream is(os.Data(), os.Size());
  int readVal;
  bool result = is.Read(readVal);

  EXPECT_TRUE(result);
  EXPECT_EQ(original, readVal);
}

// 测试pack_result和unpack_args（内部使用了read_value/write_value）
TEST(PackTest, PackUnpackBasicTypes) {
  int originalInt = 42;
  std::string originalStr = "test string";
  double originalDouble = 3.14;

  // Use pack_result which internally uses write_value
  Anycubic::Plugins::OStream os;
  Anycubic::Plugins::pack_result(&os, originalInt, originalStr, originalDouble);

  // Use unpack_args which internally uses read_value
  std::tuple<int, std::string, double> unpackedTuple;
  Anycubic::Plugins::IStream is(os.Data(), os.Size());
  bool success = Anycubic::Plugins::unpack_args(unpackedTuple, &is);

  EXPECT_TRUE(success);
  EXPECT_EQ(std::get<0>(unpackedTuple), originalInt);
  EXPECT_EQ(std::get<1>(unpackedTuple), originalStr);
  EXPECT_EQ(std::get<2>(unpackedTuple), originalDouble);
}

// 测试pack_result和unpack_args JSON类型（内部使用了read_value/write_value）
TEST(PackTest, PackUnpackJsonValue) {
  TestStruct original{42, "test name"};

  // Use pack_result which internally handles JSON serialization
  Anycubic::Plugins::OStream os;
  Anycubic::Plugins::pack_result(&os, original);

  // Use unpack_args which internally handles JSON deserialization
  std::tuple<TestStruct> unpackedTuple;
  Anycubic::Plugins::IStream is(os.Data(), os.Size());
  bool success = Anycubic::Plugins::unpack_args(unpackedTuple, &is);

  EXPECT_TRUE(success);
  EXPECT_EQ(std::get<0>(unpackedTuple).id, original.id);
  EXPECT_EQ(std::get<0>(unpackedTuple).name, original.name);
}

// 测试pack_result和unpack_args
TEST(PackTest, PackUnpackArgsTuple) {
  // Create test data
  int intVal = 123;
  std::string strVal = "hello world";
  double dblVal = 2.71;

  // Pack the data using pack_result
  Anycubic::Plugins::OStream os;
  Anycubic::Plugins::pack_result(&os, intVal, strVal, dblVal);

  // Create tuple to unpack into
  std::tuple<int, std::string, double> unpackedTuple;

  // Unpack the data
  Anycubic::Plugins::IStream is(os.Data(), os.Size());
  bool success = Anycubic::Plugins::unpack_args(unpackedTuple, &is);

  EXPECT_TRUE(success);
  EXPECT_EQ(std::get<0>(unpackedTuple), intVal);
  EXPECT_EQ(std::get<1>(unpackedTuple), strVal);
  EXPECT_EQ(std::get<2>(unpackedTuple), dblVal);
}

// 测试unpack_args_read
TEST(PackTest, UnpackArgsRead) {
  // Pack some data
  Anycubic::Plugins::OStream os;
  os.Resize(200);

  int originalInt = 999;
  std::string originalStr = "unpack test";
  float originalFloat = 1.23f;

  os.Write(originalInt);
  os.Write(originalStr);
  os.Write(originalFloat);

  // Read using unpack_args_read
  Anycubic::Plugins::IStream is(os.Data(), os.Size());
  int readInt;
  std::string readStr;
  float readFloat;

  Anycubic::Plugins::unpack_args_read(&is, readInt, readStr, readFloat);

  EXPECT_EQ(readInt, originalInt);
  EXPECT_EQ(readStr, originalStr);
  EXPECT_EQ(readFloat, originalFloat);
}

// 测试pack_free
TEST(PackTest, PackFreeCString) {
  // Create a tuple with a c string
  char *cstr = strdup("test string for pack_free");
  std::tuple<char *> testTuple(cstr);

  // Verify the string was allocated
  EXPECT_NE(std::get<0>(testTuple), nullptr);
  EXPECT_STREQ(std::get<0>(testTuple), "test string for pack_free");

  // Free the resources
  Anycubic::Plugins::pack_free(testTuple);

  // Note: After pack_free, the pointer is freed, but we can't safely access it
  // anymore
}

// 测试容器类型
TEST(PackTest, ContainerTypes) {
  std::vector<int> vec = {1, 2, 3, 4, 5};
  std::map<std::string, int> map = {{"one", 1}, {"two", 2}};

  size_t vecSize = Anycubic::Plugins::get_bytes(vec);
  EXPECT_GT(vecSize, 0);

  size_t mapSize = Anycubic::Plugins::get_bytes(map);
  EXPECT_GT(mapSize, 0);

  // Test packing/unpacking vectors using pack_result/unpack_args
  Anycubic::Plugins::OStream os;
  Anycubic::Plugins::pack_result(&os, vec);

  std::tuple<std::vector<int>> unpackedTuple;
  Anycubic::Plugins::IStream is(os.Data(), os.Size());
  bool success = Anycubic::Plugins::unpack_args(unpackedTuple, &is);

  EXPECT_TRUE(success);
  EXPECT_EQ(std::get<0>(unpackedTuple), vec);
}

// 测试边缘情况
TEST(PackTest, EdgeCases) {
  // Empty string
  std::string emptyStr = "";
  size_t emptySize = Anycubic::Plugins::get_bytes(emptyStr);
  EXPECT_EQ(emptySize, sizeof(uint16_t)); // Just the length field

  // Empty vector
  std::vector<int> emptyVec;
  size_t emptyVecSize = Anycubic::Plugins::get_bytes(emptyVec);
  EXPECT_GT(emptyVecSize,
            0); // Should still have some size for JSON representation

  // Test with zero values
  int zero = 0;
  EXPECT_EQ(Anycubic::Plugins::get_bytes(zero), sizeof(int));
}

// 保留原有的功能测试
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

TEST(DispatchCallTest, BasicFunctionality) {
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