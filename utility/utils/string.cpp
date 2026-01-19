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

#include "string.hxx"
namespace Anycubic::utility {
#ifndef NOT_WXWIDGETS
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

#endif // NOT_WXWIDGETS
} // namespace Anycubic::utility
