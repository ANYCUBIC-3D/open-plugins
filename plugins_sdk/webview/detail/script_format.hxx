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

#pragma once

#include <wx/string.h>

#include <webview/webview.h>

#include <sstream>
#include <type_traits>

template <typename _Ty> inline auto to_string(_Ty &&val) {
  using type_v = std::decay_t<_Ty>;
  if constexpr (std::is_same_v<type_v, wxString>) {
    return to_string(val.utf8_string());
  } else if constexpr (std::is_same_v<type_v, const char *> ||
                       std::is_same_v<type_v, char *>) {
    if (val == nullptr) {
      return std::string("''");
    }
    return to_string<std::string>(val);
  } else if constexpr (std::is_same_v<type_v, std::string>) {
    // 添加字符串转义
    std::string escaped;
    escaped.append(1, '\'');
    for (char c : val) {
      if (c == '\'')
        escaped += "\\'";
      else if (c == '\n')
        escaped += "\\n";
      else if (c == '\r')
        escaped += "\\r";
      else if (c == '\t')
        escaped += "\\t";
      else
        escaped += c;
    }
    escaped.append(1, '\'');
    return escaped;
  } else if constexpr (std::is_same_v<type_v, bool>) {
    return val ? "true" : "false";
  } else if constexpr (std::is_same_v<type_v, std::nullptr_t>) {
    return "null";
  } else {
    return val;
  }
}
static bool IsTestScript() {
  auto value = std::getenv("TESTS_SCRIPT");
  return value != nullptr && ::strcmp(value, "1") == 0;
}

template <typename> inline constexpr bool always_false = false;
template <typename T> wxString argument_to_string(T &&t) {
  if constexpr (std::is_same_v<std::string, std::decay_t<T>>) {
    return wxString::Format("'%s'", wxString::FromUTF8(t));
  } else if constexpr (std::is_same_v<wxString, std::decay_t<T>>) {
    return wxString::Format("'%s'", t);
  } else if constexpr (std::is_convertible_v<T, const char *>) {
    return wxString::Format("'%s'", static_cast<const char *>(t));
  } else if constexpr (std::is_arithmetic_v<std::decay_t<T>>) {
    return wxString() << t; // 统一使用流式操作
  } else {
    static_assert(always_false<T>, "Unsupported type");
    return wxEmptyString;
  }
}
template <typename... Args>
wxString ScriptFormat(const wxString &func, Args &&...args) {
  std::stringstream ss;
  static bool isTest = ::IsTestScript(); // 确保切换不太影响性能

  if (static auto value = std::getenv("SCRIPT_NAME");
      isTest && value != nullptr &&
      ::strncmp(value, func.utf8_str(), ::strlen(value)) == 0) {
    // 仅对特定的脚本进行调试
    static std::atomic_int counter = 0;
    std::string testFuncName = "test_" + std::to_string(counter++);
    // Rest/Spread参数语法 (...args)
    ss << "function " << testFuncName << "(...args) {\n"
       << "  console.group('Calling js wrapper " << func.utf8_string()
       << "');\n"
       << "  console.log('Arguments:', ...args);\n"
       << "  try {\n"
       << func.utf8_string() << "(...args);\n"
       << "  } catch (e) {\n"
       << "  debugger;\n"
       << "    console.error('Error:', e);\n"
       << "    throw e;\n"
       << "  } finally {\n"
       << "    console.groupEnd();\n"
       << "  }\n"
       << "}\n";
    ss << "debugger;\n";
    ss << testFuncName;
  } else {
    ss << func.utf8_string();
  }

  ss << "(";
  if constexpr (sizeof...(Args) > 0) {
    size_t n = 0; // 自动添加逗号分隔
    ((ss << (n++ ? ", " : "") << to_string(std::forward<Args>(args))), ...);
  }
  ss << ");";
  auto script = ss.str();
  return wxString::FromUTF8(script.data(), script.size());
}
template <typename... Args> wxString ArgumentFormat(Args &&...args) {
  wxArrayString result;
  if constexpr (sizeof...(args) > 0) {
    (result.Add(argument_to_string(std::forward<Args>(args))), ...);
  }
  return wxJoin(result, ',');
}

static bool RunScript(wxWebView *webView, const wxString &javascript) {
  try {
    if (wxIsMainThread()) {
      webView->RunScriptAsync(javascript);
    } else {
      webView->CallAfter(
          [webView, javascript]() { webView->RunScriptAsync(javascript); });
    }
    return true;
  } catch (std::exception &) {
    return false;
  }
}
