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
#ifndef NOT_WXWIDGETS
#include <wx/string.h>
namespace Anycubic::utility {
/**
 * @brief 拼接路径
 *
 * @param root 根路径
 * @param sub 子路径
 * @return wxString 拼接后的路径
 */
wxString JoinPath(const wxString &root, const wxString &sub);
/**
 * @brief 拼接文件名
 *
 * @param root 根路径
 * @param filename 文件名
 * @return wxString 拼接后的路径
 */
wxString JoinFilename(const wxString &root, const wxString &filename);

/**
 * @brief 判断路径是否存在
 *
 * @param path 路径
 * @return true 路径存在
 * @return false 路径不存在
 */
bool PathExists(const wxString &path);

/**
 * @brief 判断文件是否存在
 *
 * @param path 文件路径
 * @return true 文件存在
 * @return false 文件不存在
 */
bool FileExists(const wxString &path);

/**
 * @brief 创建路径
 *
 * @param path 路径
 * @return true 路径创建成功
 * @return false 路径创建失败
 */
bool CreatePaths(const wxString &path);
} // namespace Anycubic::utility
#endif // NOT_WXWIDGETS