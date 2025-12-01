#pragma once

#include <utility>
#include <vector>

#include "wx/defs.h"

#ifndef wxUSE_FILESYSTEM
#error "wxUSE_FILESYSTEM must be defined"
#endif // wxUSE_FILESYSTEM

#include "wx/filesys.h"

class MemoryFSHandler : public wxFileSystemHandler {
public:
  bool AddFS(const wxString &name, const wxString &xrc);
  bool DelFS(const wxString &name);

public:
  // wxFileSystemHandler override
  bool CanOpen(const wxString &location) override;
  wxFSFile *OpenFile(wxFileSystem &fs, const wxString &location) override;

private:
  using node_type = std::pair<wxString, wxString>;
  std::vector<node_type> m_fs_nodes;
  static wxDateTime m_last_modified;
};