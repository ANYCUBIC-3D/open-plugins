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
  int64_t version;
  int64_t build_time;
  map_type files;
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