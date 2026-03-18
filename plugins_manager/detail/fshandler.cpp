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

#include "fshandler.hpp"

#include <wx/filesys.h>
#include <wx/sstream.h>
wxDateTime MemoryFSHandler::m_last_modified = wxDateTime::Now();
bool MemoryFSHandler::AddFS(const wxString &name, const wxString &xrc) {
  if (auto itr = std::ranges::find_if(
          m_fs_nodes,
          [&name](const node_type &node) { return node.first == name; });
      itr != m_fs_nodes.end()) {
    return false;
  }
  m_fs_nodes.emplace_back(name, xrc);
  return true;
}

bool MemoryFSHandler::DelFS(const wxString &name) {
  return std::erase_if(m_fs_nodes, [&name](const node_type &node) {
           return node.first == name;
         }) > 0;
}

bool MemoryFSHandler::CanOpen(const wxString &location) {
  return GetProtocol(location) == wxT("acmemory");
}

wxFSFile *MemoryFSHandler::OpenFile(wxFileSystem &fs,
                                    const wxString &location) {
  auto right_location = GetRightLocation(location);
  auto i = std::ranges::find_if(m_fs_nodes,
                                [&right_location](const node_type &node) {
                                  return node.first == right_location;
                                });
  if (i == m_fs_nodes.end())
    return nullptr;
  auto stream = new wxStringInputStream(i->second);
  return new wxFSFile(stream, location, wxEmptyString, GetAnchor(location),
                      m_last_modified);
}
