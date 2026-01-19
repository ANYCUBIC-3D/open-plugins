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

#include "filesystem.hxx"
#ifndef NOT_WXWIDGETS
#include <wx/filename.h>

namespace Anycubic::utility {
wxString JoinPath(const wxString &root, const wxString &sub) {
  if (root.IsEmpty()) {
    return sub;
  } else if (sub.IsEmpty()) {
    return root;
  }
  auto root_dir = wxFileName::DirName(root);
  auto new_path = wxFileName::DirName(sub);
  for (auto v : new_path.GetDirs()) {
    root_dir.AppendDir(v);
  }
  root_dir.MakeAbsolute();
  return root_dir.GetPath();
}

wxString JoinFilename(const wxString &root, const wxString &filename) {
  if (root.IsEmpty()) {
    return filename;
  } else if (filename.IsEmpty()) {
    return root;
  }
  auto root_dir = wxFileName::DirName(root);
  auto new_path = wxFileName::FileName(filename);
  for (auto v : new_path.GetDirs()) {
    root_dir.AppendDir(v);
  }
  root_dir.SetFullName(new_path.GetFullName());
  root_dir.MakeAbsolute();
  return root_dir.GetFullPath();
}

bool PathExists(const wxString &path) { return wxFileName::DirExists(path); }

bool FileExists(const wxString &path) { return wxFileName::FileExists(path); }

bool CreatePaths(const wxString &path) {
  wxFileName dirName(path);
  return dirName.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
}
} // namespace Anycubic::utility
#endif // NOT_WXWIDGETS
