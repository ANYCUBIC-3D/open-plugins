// Copyright (c) 2026 深圳市纵维立方科技有限公司
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

#include <map>
#include <string>
#include <vector>

#include <wx/string.h>

using map_type = std::map<std::string, std::string, std::less<>>;

bool LoadSignture(const char *plagin, std::vector<char> &buffer);
bool Decode(std::vector<char> &buffer);
bool Eecode(std::vector<char> &buffer);
struct Package {
  int64_t version;    ///< 插件包版本号
  int64_t build_time; ///< 编译时间
  std::string name;   ///< 插件包名
  map_type files;     ///< 插件包文件列表
};

bool ParseInfo(const std::vector<char> &buffer, Package *info);
bool SaveInfo(const Package &info, std::vector<char> &buffer);

bool LoadMD5(const char *plagin, map_type &md5s);

bool IsPlugins(const wxString &filename);
wxString GetPluginName(const wxString &filename);

bool Unzip(const std::string &zip, const std::string &dir,
           bool remove_first = true);
bool UnzipAll(const std::vector<wxString> &plugins_packages,
              const std::string &tmp_dir);
bool Zip(const std::string &dir, const std::string &zip);