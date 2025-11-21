#pragma once

#include <wx/string.h>
namespace Anycubic::utility {
wxString JoinPath(const wxString &root, const wxString &sub);
wxString JoinFilename(const wxString &root, const wxString &filename);
bool PathExists(const wxString &path);
bool FileExists(const wxString &path);
bool CreatePaths(const wxString &path);
} // namespace Anycubic::utility