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

#include "package.hxx"
#include "plaform.hpp"

#include <easy_log/stackstrace.hxx>
#include <utility/codec/bin2ascii.hxx>
#include <utility/codec/md5.hxx>
#include <utility/encrypt/aes.hxx>

#include <iguana/detail/string_stream.hpp>
#include <iguana/json.hpp>

#include <wx/dir.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#include <boost/scope_exit.hpp>

REFLECTION(Package, version, build_time, name, files)
bool LoadSignture(const char *plugins, std::vector<char> &buffer) {
  FUNC_ENTRY2("plugins = {}", plugins);
  // 步骤1：打开zip文件
  wxFileInputStream fileStream(wxString::FromUTF8(plugins));
  if (!fileStream.IsOk()) {
    FUNC_LEAVE;
    return false; // 文件打开失败
  }

  // 步骤2：创建zip输入流
  wxZipInputStream zipStream(fileStream);
  wxZipEntry *entry = nullptr;

  // 步骤3：遍历zip条目查找签名文件（签名文件名为"signature.bin"）
  while ((entry = zipStream.GetNextEntry()) != nullptr) {
    BOOST_SCOPE_EXIT(entry) { delete entry; }
    BOOST_SCOPE_EXIT_END
    if (entry->GetName() == "signature.bin") {
      // 步骤4：读取签名数据
      size_t signSize = entry->GetSize();
      if (signSize == 0) {
        FUNC_LEAVE;
        return false; // 签名文件为空
      }
      // 分配内存存储签名数据（需与PluginsPackageInfo结构匹配）
      buffer.resize(signSize + 1);
      zipStream.Read(buffer.data(), signSize);
      FUNC_LEAVE;
      return true; // 成功找到并读取签名
    }
  }
  FUNC_LEAVE;
  return false;
}

bool Decode(std::vector<char> &buffer) {
  FUNC_ENTRY;
  auto str =
      ::aesDecrypt(std::string(buffer.data(), buffer.size()), AES_PASSWORD);
  if (str.empty()) {
    FUNC_LEAVE;
    return false;
  }
  buffer.assign(str.begin(), str.end());
  FUNC_LEAVE;
  return true;
}

bool Eecode(std::vector<char> &buffer) {
  FUNC_ENTRY;
  auto str =
      ::aesEncrypt(std::string(buffer.data(), buffer.size()), AES_PASSWORD);
  if (str.empty()) {
    FUNC_LEAVE;
    return false;
  }
  buffer.assign(str.begin(), str.end());
  FUNC_LEAVE;
  return true;
}

bool ParseInfo(const std::vector<char> &buffer, Package *info) {
  FUNC_ENTRY;
  try {
    iguana::json::reader_t reader(buffer.data(), buffer.size());
    iguana::json::read_json(reader, *info);
    FUNC_LEAVE;
    return true;
  } catch (const std::exception &e) {
    FUNC_LEAVE2("failed,errno={1}, msg={0}", e.what());
    return false;
  }
}

bool SaveInfo(const Package &info, std::vector<char> &buffer) {
  FUNC_ENTRY;
  try {
    iguana::string_stream ss;
    iguana::json::to_json(ss, info);
    auto json_str = ss.str();
    // 加密数据
    auto encrypted = ::aesEncrypt(json_str, AES_PASSWORD);
    if (encrypted.empty()) {
      FUNC_LEAVE;
      return false;
    }

    // 写入buffer
    buffer.assign(encrypted.begin(), encrypted.end());
    FUNC_LEAVE;
    return true;
  } catch (const std::exception &e) {
    FUNC_LEAVE2("SaveInfo failed: {}", e.what());
    return false;
  }
}

bool LoadMD5(const char *plugins, map_type &md5s) {
  FUNC_ENTRY;
  // 步骤1：打开zip文件
  wxFileInputStream fileStream(wxString::FromUTF8(plugins));
  if (!fileStream.IsOk()) {
    FUNC_LEAVE;
    return false; // 文件打开失败
  }

  // 步骤2：创建zip输入流
  wxZipInputStream zipStream(fileStream);
  wxZipEntry *entry = nullptr;

  std::string buffer(1024 * 1024, 0); // 1MB的缓冲区
  // 步骤3：遍历zip条目查找签名文件（签名文件名为"signature.bin"）
  while ((entry = zipStream.GetNextEntry()) != nullptr) {
    BOOST_SCOPE_EXIT(entry) { delete entry; }
    BOOST_SCOPE_EXIT_END

    if (auto filename = entry->GetName(); ::IsPlugins(entry->GetName())) {

      cMd5 md5sum;
      size_t totalRead = entry->GetSize();
      while (totalRead > 0) {
        size_t toRead = (std::min)(buffer.size(), totalRead);
        size_t read = zipStream.Read(buffer.data(), toRead).LastRead();
        // if (read == 0) {
        //   break;
        //   //理论不出现的，每次都判断一下有点丑，选写在这里如果有问题，去除注释
        // }
        totalRead -= read;
        md5sum.write(buffer.data(), read);
      }
      md5sum.sum(buffer.data() + 32);
      ::bin2hex(buffer.data(), buffer.data() + 32, MD5LEN);
      buffer[32] = '\0';
      md5s.try_emplace(filename.utf8_string(), buffer);
    }
  }
  FUNC_LEAVE;
  return !md5s.empty();
}

bool IsPlugins(const wxString &filename) {
  auto fname = wxFileName::FileName(filename).GetFullName();
  return (fname.StartsWith(PLUGIN_PREFIX) &&
          fname.Lower().EndsWith(PLUGIN_EXT));
}

wxString GetPluginName(const wxString &filename) {
  FUNC_ENTRY2("filename = {}", filename.utf8_string());
  if (!::IsPlugins(filename)) {
    FUNC_LEAVE;
    return wxString();
  }
  auto name = wxFileName::FileName(filename).GetName();
  name.Replace(PLUGIN_PREFIX, wxASCII_STR(""));
#ifndef NDEBUG
  // 移除d的标记
  name = name.RemoveLast(1);
#endif
  FUNC_LEAVE;
  return name;
}

bool Unzip(const std::string &in_zip, const std::string &out_dir,
           bool remove_first) {
  FUNC_ENTRY2("in_zip = {}, out_dir = {}", in_zip, out_dir);
  auto dir = wxFileName::DirName(wxString::FromUTF8(out_dir));

  if (remove_first && dir.DirExists()) {
    dir.Rmdir(wxPATH_RMDIR_RECURSIVE); // 添加递归删除确保清空目录
  }
  if (!dir.DirExists())
    dir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL); // 使用完整路径创建模式

  wxFileInputStream in(wxString::FromUTF8(in_zip));
  if (!in.IsOk()) {
    FUNC_LEAVE;
    return false; // 添加输入流检查
  }

  wxZipInputStream zip(in);
  if (!zip.IsOk()) {
    FUNC_LEAVE;
    return false; // 添加zip流检查
  }

  wxZipEntry *entry;
  bool hasError = false;

  while ((entry = zip.GetNextEntry()) != nullptr) {

    BOOST_SCOPE_EXIT(entry) { delete entry; }
    BOOST_SCOPE_EXIT_END

    // 统一使用平台路径分隔符
    wxString entryName = entry->GetName();
    if (!IsPlugins(entryName)) {
      continue;
    }
    entryName.Replace(wxT("/"), wxFileName::GetPathSeparator());

    wxFileName outputPath = dir;
    outputPath.AppendDir(entryName);

    // 处理目录创建
    if (entry->IsDir()) {
      if (!outputPath.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
        hasError = true;
        break;
      }
      continue;
    }

    // 创建父目录
    if (!outputPath.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
      hasError = true;
      break;
    }

    wxFileOutputStream out(outputPath.GetFullPath());
    if (!out.IsOk()) {
      hasError = true;
      break;
    }

    zip.Read(out);
    if (zip.GetLastError() != wxSTREAM_NO_ERROR) { // 添加流错误检查
      hasError = true;
      break;
    }
  }
  FUNC_LEAVE;
  return !hasError; // 返回实际解压状态
}

bool Zip(const std::string &dir, const std::string &zip_path) {
  FUNC_ENTRY2("dir = {}, zip_path = {}", dir, zip_path);
  wxFileOutputStream out(wxString::FromUTF8(zip_path));
  if (!out.IsOk()) {
    FUNC_LEAVE;
    return false; // 添加输出流检查
  }

  wxZipOutputStream zip(out);
  if (!zip.IsOk()) {
    FUNC_LEAVE;
    return false; // 添加zip流检查
  }

  wxDir traverser;
  if (!traverser.Open(wxString::FromUTF8(dir))) {
    FUNC_LEAVE;
    return false; // 添加目录遍历检查
  }

  wxString filename;
  bool hasError = false;

  // 递归遍历目录
  for (bool cont = traverser.GetFirst(&filename, ::wxEmptyString,
                                      wxDIR_FILES | wxDIR_DIRS);
       cont && !hasError; cont = traverser.GetNext(&filename)) {
    hasError = true;

    if (wxFileName file_path(wxString::FromUTF8(dir), filename);
        file_path.IsDir()) {
      // 添加目录条目
      auto entry = std::make_unique<wxZipEntry>(file_path.GetFullName() +
                                                wxFileName::GetPathSeparator());
      entry->SetIsDir();
      if (!zip.PutNextEntry(entry.get())) {
        break;
      }
    } else {
      // 添加文件条目
      wxFileInputStream in(file_path.GetFullPath());
      if (!in.IsOk()) {
        break;
      }

      if (auto entry = std::make_unique<wxZipEntry>(file_path.GetFullName());
          !zip.PutNextEntry(entry.get())) {
        break;
      }

      zip.Write(in);
      if (zip.GetLastError() != wxSTREAM_NO_ERROR) {
        break;
      }
    }
    hasError = false;
  }

  zip.Close();
  FUNC_LEAVE;
  return !hasError;
}

bool UnzipAll(const std::vector<wxString> &plugins_packages,
              const std::string &tmp_dir) {
  FUNC_ENTRY2("plugins_packages = {}, tmp_dir = {}", plugins_packages.size(),
              tmp_dir);
  // 创建临时目录
  auto dir = wxFileName::DirName(wxString::FromUTF8(tmp_dir));
#ifdef NDEBUG
  if (dir.DirExists()) {
    LOG_INFO("tmp_dir:{} already exists, remove it", tmp_dir);
    dir.Rmdir(wxPATH_RMDIR_RECURSIVE);
  }
  dir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
#endif
  for (auto package : plugins_packages) {
    // 解压单个zip文件
    if (!Unzip(package.utf8_string(), tmp_dir, false)) {
      // 解压失败，返回false
      FUNC_LEAVE;
      return false;
    }
  }
  // 所有文件解压成功，返回true
  FUNC_LEAVE;
  return true;
}