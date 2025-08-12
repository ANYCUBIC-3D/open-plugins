#include "package.hpp"
#include "plaform.hpp"

#include <easy_log/stackstrace.hxx>
#include <utility/codec/bin2ascii.hxx>
#include <utility/codec/md5.hxx>
#include <utility/encrypt/aes.hxx>

#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#include <boost/json.hpp>
#include <boost/pfr.hpp>
#include <boost/scope_exit.hpp>

bool LoadSignture(const char *plugins, std::vector<char> &buffer) {
  // 步骤1：打开zip文件
  wxFileInputStream fileStream(wxString::FromUTF8(plugins));
  if (!fileStream.IsOk()) {
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
        return false; // 签名文件为空
      }
      // 分配内存存储签名数据（需与PluginsPackageInfo结构匹配）
      buffer.resize(signSize + 1);
      zipStream.Read(buffer.data(), signSize);
      return true; // 成功找到并读取签名
    }
  }
  return false;
}

bool Decode(std::vector<char> &buffer) {
  auto str =
      aesDecrypt(std::string(buffer.data(), buffer.size()), AES_PASSWORD);
  if (str.empty())
    return false;
  buffer.assign(str.begin(), str.end());
  return true;
}
namespace boost::json {
// 反序列化
template <typename T>
static T tag_invoke(const value_to_tag<T> &, const value &jv) {
  T c;
  using type_v = std::decay_t<T>;
  auto &tid = typeid(type_v);
  FUNC_ENTRY2("type name:{}", tid.name());
  try {
    auto names = boost::pfr::names_as_array<T>();
    auto &jo = jv.as_object();
    boost::pfr::for_each_field(c, [&jo, &names](auto &field, auto index) {
      FUNC_ENTRY2("field={}", std::string(names[index]));
      using value_type = std::decay_t<decltype(field)>;
      auto itr = jo.find(names[index]);
      if (itr == jo.end() || itr->value().is_null()) {
        FUNC_LEAVE2("field is null or not found");
        return;
      }
      if constexpr (std::is_same_v<value_type, int64_t>) {
        field = itr->value().as_int64();
      } else {
        auto v = itr->value().as_object();
        for (auto &[k, val] : v) {
          field[k] = val.as_string();
        }
      }
      FUNC_LEAVE
    });
  } catch (boost::system::system_error &e) {
    FUNC_LEAVE2("failed,errno={1}, msg={0}", e.what(), e.code().value());
  }
  return c;
}
} // namespace boost::json

bool ParseInfo(const std::vector<char> &buffer, Package *info) {

  boost::json::value json =
      boost::json::parse(std::string(buffer.begin(), buffer.end()));
  if (!json.is_object()) {
    return false;
  }
  try {
    *info = boost::json::value_to<Package>(json);
    return true;
  } catch (std::exception &e) {
    FUNC_LEAVE2("failed,errno={1}, msg={0}", e.what());
    return false;
  }
}

bool LoadMD5(const char *plugins, std::map<std::string, std::string> &md5s) {
  // 步骤1：打开zip文件
  wxFileInputStream fileStream(wxString::FromUTF8(plugins));
  if (!fileStream.IsOk()) {
    return false; // 文件打开失败
  }

  // 步骤2：创建zip输入流
  wxZipInputStream zipStream(fileStream);
  wxZipEntry *entry = nullptr;

  char buffer[1024 * 1024] = {0}; // 1MB
  // 步骤3：遍历zip条目查找签名文件（签名文件名为"signature.bin"）
  while ((entry = zipStream.GetNextEntry()) != nullptr) {
    BOOST_SCOPE_EXIT(entry) { delete entry; }
    BOOST_SCOPE_EXIT_END

    if (auto filename = entry->GetName(); IsPlugins(entry->GetName())) {

      cMd5 md5sum;
      size_t totalRead = entry->GetSize();
      while (totalRead > 0) {
        size_t toRead = (std::min)(sizeof(buffer), totalRead);
        size_t read = zipStream.Read(buffer, toRead).LastRead();
        // if (read == 0) {
        //   break;
        //   //理论不出现的，每次都判断一下有点丑，选写在这里如果有问题，去除注释
        // }
        totalRead -= read;
        md5sum.write(buffer, read);
      }
      md5sum.sum(buffer + 32);
      bin2hex(buffer, buffer + 32, MD5LEN);
      buffer[32] = '\0';
      md5s.emplace(filename.utf8_string(), buffer);
    }
  }
  return !md5s.empty();
}

bool IsPlugins(const wxString &filename) {
  return (filename.StartsWith(wxASCII_STR("plugin")) &&
          filename.Lower().EndsWith(PLUGIN_EXT));
}

wxString GetPluginName(const wxString &filename) {
  if (!IsPlugins(filename)) {
    return wxString();
  }
  auto name = filename.BeforeFirst(wxT('.'));
  name.Replace(wxASCII_STR("plugin"), wxASCII_STR(""));
  return name;
}

bool Unzip(const std::string &in_zip, const std::string &out_dir) {
  auto dir = wxFileName::DirName(wxString::FromUTF8(out_dir));
  if (dir.DirExists()) {
    dir.Rmdir(wxPATH_RMDIR_RECURSIVE); // 添加递归删除确保清空目录
  }
  dir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL); // 使用完整路径创建模式

  wxFileInputStream in(wxString::FromUTF8(in_zip));
  if (!in.IsOk())
    return false; // 添加输入流检查

  wxZipInputStream zip(in);
  if (!zip.IsOk())
    return false; // 添加zip流检查

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
  return !hasError; // 返回实际解压状态
}
