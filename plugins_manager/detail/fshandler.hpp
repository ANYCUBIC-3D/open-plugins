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