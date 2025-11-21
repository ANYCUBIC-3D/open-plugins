#include "filesystem.hxx"

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
