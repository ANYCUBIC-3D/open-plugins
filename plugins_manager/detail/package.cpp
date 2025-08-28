#include "package.hxx"
#include "plaform.hpp"

#include <easy_log/stackstrace.hxx>
#include <utility/codec/bin2ascii.hxx>
#include <utility/codec/md5.hxx>
#include <utility/encrypt/aes.hxx>

#include <wx/dir.h>
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
      ::aesDecrypt(std::string(buffer.data(), buffer.size()), AES_PASSWORD);
  if (str.empty())
    return false;
  buffer.assign(str.begin(), str.end());
  return true;
}

bool Eecode(std::vector<char> &buffer) {
  auto str =
      ::aesEncrypt(std::string(buffer.data(), buffer.size()), AES_PASSWORD);
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
// 序列化
template <typename T>
void tag_invoke(const value_from_tag &, value &jv, T const &t) {
  auto names = boost::pfr::names_as_array<T>();
  object obj;

  boost::pfr::for_each_field(t, [&obj, &names](auto &field, auto index) {
    using value_type = std::decay_t<decltype(field)>;

    if constexpr (std::is_same_v<value_type, int64_t>) {
      obj[names[index]] = field;
    } else {
      object nested;
      for (auto &[k, v] : field) {
        nested.emplace(k, v);
      }
      obj[names[index]] = nested;
    }
  });

  jv = obj;
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
  } catch (const boost::json::system_error &e) {
    FUNC_LEAVE2("failed,errno={1}, msg={0}", e.what());
    return false;
  }
}

bool SaveInfo(const Package &info, std::vector<char> &buffer) {
  try {
    // 将结构体序列化为JSON
    boost::json::value json = boost::json::value_from(info);

    // 序列化JSON字符串
    std::string json_str = boost::json::serialize(json);

    // 加密数据
    auto encrypted = ::aesEncrypt(json_str, AES_PASSWORD);
    if (encrypted.empty()) {
      return false;
    }

    // 写入buffer
    buffer.assign(encrypted.begin(), encrypted.end());
    return true;
  } catch (const boost::json::system_error &e) {
    FUNC_LEAVE2("SaveInfo failed: {}", e.what());
    return false;
  }
}

bool LoadMD5(const char *plugins, map_type &md5s) {
  // 步骤1：打开zip文件
  wxFileInputStream fileStream(wxString::FromUTF8(plugins));
  if (!fileStream.IsOk()) {
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
  return !md5s.empty();
}

bool IsPlugins(const wxString &filename) {
  return (filename.StartsWith(wxASCII_STR("plugin")) &&
          filename.Lower().EndsWith(PLUGIN_EXT));
}

wxString GetPluginName(const wxString &filename) {
  if (!::IsPlugins(filename)) {
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

bool Zip(const std::string &dir, const std::string &zip_path) {
  wxFileOutputStream out(wxString::FromUTF8(zip_path));
  if (!out.IsOk())
    return false;

  wxZipOutputStream zip(out);
  if (!zip.IsOk())
    return false;

  wxDir traverser;
  if (!traverser.Open(wxString::FromUTF8(dir)))
    return false;

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
  return !hasError;
}