#include "string.hxx"
namespace Anycubic::utility {
wxString FromUtf8(const std::string &str) { return wxString::FromUTF8(str); }

std::string ToUtf8(const wxString &str) { return str.utf8_string(); }

#ifndef NDEBUG
void test() {
  std::vector<std::string> items = {"a", "b", "c"};
  auto joined = Join(items, ",");
  assert(joined == "a,b,c");
}
void test2() {
  std::string str = "a,b,c";
  std::vector<std::string> items;
  Split(std::back_inserter(items), str, ",");
  assert(items.size() == 3);
  assert(items[0] == "a");
  assert(items[1] == "b");
  assert(items[2] == "c");
}
#endif
} // namespace Anycubic::utility
