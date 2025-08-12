#pragma once

#include <map>
#include <string>
#include <vector>

#include <wx/string.h>

bool LoadSignture(const char *plagin, std::vector<char> &buffer);
bool Decode(std::vector<char> &buffer);

struct Package {
  int64_t version;
  int64_t build_time;
  std::map<std::string, std::string> files;
};

bool ParseInfo(const std::vector<char> &buffer, Package *info);

bool LoadMD5(const char *plagin, std::map<std::string, std::string> &md5s);

bool IsPlugins(const wxString &filename);
wxString GetPluginName(const wxString &filename);

bool Unzip(const std::string &zip, const std::string &dir);
