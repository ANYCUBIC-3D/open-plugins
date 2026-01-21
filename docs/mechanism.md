# 机制解析

## router - 插件路由机制

### 核心特性
- **零配置注册**：`REGISTER_FUNCATION(MyPlugin, func)` 自动处理序列化
- **类型安全**：编译期类型检查，运行时自动序列化
- **同步调用**：`dispatch_call`为同步阻塞调用

### 关键限制
1. **注册时机**：函数注册必须在插件初始化阶段完成
2. **字符串长度**：最大支持65535字符，超长触发assert失败
3. **指针处理**：仅复制指针值，不深度复制数据

### 工作流程

#### 注册阶段
```cpp
#define REGISTER_FUNCATION(type, func) \
  AddFunction(BOOST_PP_STRINGIZE(func), Anycubic::Plugins::make_call(&type::func, this))
```
- **插件初始化**：设置插件名称前缀
- **宏展开**：自动生成类型安全的模板调用链
- **包装器生成**：生成处理`IStream`/`OStream`的调用包装器

#### 调用阶段
```cpp
int result = dispatch_call<int>(router, "plugin", "calculate", 10, 20);
```
- **同步执行**：阻塞当前线程直到函数完成
- **自动序列化**：参数和返回值自动处理

### 使用示例

#### 函数注册
```cpp
class MyPlugin {
public:
    int calculate(int a, int b) { return a + b; }
    void registerFunctions(PluginRouter* router) {
        router->REGISTER_FUNCATION(MyPlugin, calculate);
    }
};
```

### 技术实现

#### 序列化机制
- **类型统一**：`wxString`/`std::string`/`const char*`统一转换为`std::string`
- **指针优化**：仅复制指针值，避免深度拷贝

#### 编译期处理
```cpp
// 编译期自动处理参数解包和返回值序列化
if constexpr (arg_count > 0) unpack_args(args, data);
ret_type ret = std::apply(func, std::tuple_cat(std::make_tuple(self), args));
pack_result(result, ret);
```

### 性能建议
- **小型数据**：直接传值（int、float等）
- **大型结构体**：使用指针传递避免拷贝
- **动态数据**：确保生命周期覆盖调用过程

### 设计优势
- **开发友好**：隐藏复杂模板细节，提供简单宏接口
- **编译期安全**：类型错误在编译期捕获
- **运行时高效**：最小化运行时开销

## event - 插件事件机制

### 核心特性
- **通用事件包装**：`wxPluginEvent`作为所有插件事件的统一包装类
- **键值对数据**：支持通过键值对传递简单的事件数据
- **共享数据支持**：支持设置共享数据指针和自定义析构函数
- **状态管理**：内置成功状态和错误码管理机制

### 关键接口详解

#### SharedData机制
```cpp
// 共享数据设置接口
void SetSharedData(void *data, void *ctx, void (*dtor)(void *, void *));

// 共享数据获取接口  
void *GetSharedData(void) const;
```

**SharedData设计原理：**
- **数据所有权**：事件对象不拥有数据，仅持有指针引用
- **生命周期管理**：通过自定义析构函数确保资源正确释放
- **上下文关联**：支持传递上下文信息，便于资源管理

#### 键值对数据接口
```cpp
// 数据存取接口
wxString GetValue(const wxString &key) const;
void SetValue(const wxString &key, const wxString &value);
```

#### 状态管理接口
```cpp
// 状态管理
bool IsSuccess(void) const;
void SetSuccess(bool success);
int32_t GetCode(void) const;
void SetCode(int32_t code);
```

### SharedData使用模式

#### 模式1：简单数据共享
```cpp
// 设置共享数据（无自定义析构）
MyData* data = new MyData{value1, value2};
event.SetSharedData(data, nullptr, [](void* data, void* ctx) {
    delete static_cast<MyData*>(data);
});

// 接收方获取数据
MyData* receivedData = static_cast<MyData*>(event.GetSharedData());
```

### 设计优势

#### SharedData核心优势
- **内存安全**：自定义析构函数确保资源正确释放
- **零拷贝传输**：仅传递指针，避免大数据拷贝开销
- **类型灵活**：支持任意类型的数据共享
- **生命周期可控**：通过上下文管理复杂的资源关系

#### 通用性设计
- **统一接口**：所有插件使用相同的事件机制
- **扩展性强**：通过键值对和SharedData支持各种业务场景
- **类型安全**：编译期检查事件类型和接口使用

#### 性能优化
- **轻量级事件**：事件对象本身开销小
- **按需数据传递**：简单数据用键值对，复杂数据用SharedData
- **异步安全**：支持跨线程事件传递和数据处理

## i18n - 国际化机制

### 核心特性
- **多语言支持**：内置中文、英文等多语言翻译
- **动态切换**：运行时动态切换语言无需重启应用
- **翻译宏**：提供`_()`、`_L()`、`_utf8()`等便捷翻译宏
- **上下文支持**：支持带上下文的翻译，解决一词多义问题

### 代码实现

#### 翻译宏定义
```cpp
// 基础翻译宏
#define _(s) Anycubic::Plugins::I18n::translate((s))
#define _L(s) Anycubic::Plugins::I18n::translate((s))
#define _utf8(s) Anycubic::Plugins::I18n::translate_utf8((s))
#define _u8L(s) Anycubic::Plugins::I18n::translate_utf8((s))

// 上下文翻译宏
#define _CTX(s, ctx) Anycubic::Plugins::I18n::translate((s), (ctx))
#define _CTX_utf8(s, ctx) Anycubic::Plugins::I18n::translate_utf8((s), (ctx))

// 开发辅助宏
#define _devL(s) wxString((s))        // 开发阶段不翻译
#define _omitL(s) ("")                // 忽略翻译
#define L(s) s                        // 标记可本地化字符串
```


### CMake构建支持

#### 启用配置
i18n支持通过CMake选项`ENABLE_I18N`控制，默认启用：
```cmake
option(ENABLE_I18N "Enable i18n support" ON)
```

#### 核心构建函数

**i18n_generate_pot函数**
- **功能**：生成POT翻译模板文件和PO翻译文件
- **参数**：目标项目、POT文件路径、PO文件根目录、语言代码列表
- **示例**：`i18n_generate_pot(${target} ${POT_FILE} ${target_source_dir}/i18n/ zh_CN en_US)`

**i18n_generate_mo函数**
- **功能**：将PO文件编译为MO二进制文件
- **参数**：PO文件根目录
- **示例**：`i18n_generate_mo(${target_source_dir}/i18n/)`

**i18n_generate_embed函数**
- **功能**：将MO文件嵌入到目标程序中
- **参数**：目标项目、PO文件根目录
- **示例**：`i18n_generate_embed(${target} ${target_source_dir}/i18n/)`

#### 构建流程
1. **字符串提取**：使用xgettext工具扫描源代码中的翻译字符串
2. **模板生成**：生成POT翻译模板文件
3. **翻译文件创建**：为每种语言生成PO文件
4. **二进制编译**：将PO文件编译为MO二进制格式
5. **资源嵌入**：将MO文件嵌入到程序中

### 使用流程

#### 1. 代码中使用翻译宏
```cpp
// 基础使用
wxString title = _("Plugin Title");
std::string message = _L("Welcome to the plugin");

// 带上下文的使用（解决一词多义）
wxString saveFile = _CTX("Save", "File operation");
wxString saveGame = _CTX("Save", "Game operation");

// UTF-8编码翻译
std::string utf8Msg = _utf8("UTF-8 message");
```

#### 2. CMake配置
```cmake
# 包含i18n支持模块
include(generate.cmake)

# 为目标启用i18n支持
i18n_generate_pot(${target} ${POT_FILE} ${target_source_dir}/i18n/ zh_CN en_US)
i18n_generate_mo(${target_source_dir}/i18n/)
i18n_generate_embed(${target} ${target_source_dir}/i18n/)
```

#### 3. 运行时支持
```cpp
// 插件初始化时加载翻译
acTranslationsLoader loader;
loader.LoadFromMemory(mo_data, mo_size, mo_md5);

// 或从文件加载
loader.LoadFromFile("/usr/share/locale/zh_CN/LC_MESSAGES/anycubic.mo");
```

### 设计优势
- **开发友好**：提供多种翻译宏满足不同场景需求
- **编译期安全**：类型安全的翻译函数接口
- **运行时高效**：基于wxWidgets成熟的国际化框架
- **构建集成**：完整的CMake构建支持，自动化翻译流程
- **灵活配置**：支持按需启用/禁用i18n功能

## xrc - 资源管理机制

### 核心特性
- **内存资源管理**：支持从内存数据加载XRC资源
- **虚拟文件系统**：通过MemoryFSHandler管理内存中的资源文件
- **动态加载**：运行时动态加载和卸载界面资源

### 技术实现
```cpp
// 内存文件系统处理器
class MemoryFSHandler : public wxFileSystemHandler {
public:
    bool AddFile(const wxString& filename, const void* data, size_t size);
    bool RemoveFile(const wxString& filename);
};
```

## wxWebView 


### 核心组件详解

#### 1. 路由基础架构 (RouterBase)

路由是WebView机制的核心，支持完整的HTTP方法：

```cpp
class RouterBase {
public:
    enum METHOD_TYPE {
        MethodGET = 0,    // 数据查询
        MethodPOST,       // 数据创建  
        MethodDELETE,     // 数据删除
        MethodPUT,        // 数据更新
        MethodOPTIONS     // CORS预检
    };
    
    virtual bool RegisterHandler(METHOD_TYPE method, 
                                const wxString &pattern,
                                HandlerBase *handler) = 0;
};
```

**路由匹配策略**：
- **精确匹配**：路径完全相等时优先匹配
- **正则匹配**：支持复杂路径模式匹配

#### 2. API处理器 (WebviewApiHandler)

作为前端与业务逻辑的桥梁，提供实例管理和请求分发：

```cpp
class WebviewApiHandler : public wxWebViewHandler {
private:
    wxSharedPtr<RouterBase> m_router;        // 核心路由
    std::vector<InstanceBase*> m_instances;  // 业务实例集合
    
public:
    // 模板方法创建并管理业务实例
    template <typename _Ty, typename... Args>
    _Ty* AddInstanceT(Args&&... args) {
        _Ty* instance = new _Ty(std::forward<Args>(args)...);
        AddInstance(instance);
        return instance;
    }
};
```

## wxWebView机制

### 核心特性

wxWebView机制为插件系统提供Web API服务能力。主要特性包括：

- **路由注册**：支持GET/POST/DELETE/PUT/OPTIONS方法的路由注册
- **请求处理**：统一的HTTP请求处理响应机制

### 基础架构

#### 核心组件

**RouterBase类** - 路由基类，提供基础路由接口功能。

**WebviewApiHandler类** - API处理器，管理业务实例并通过RouterBase处理HTTP请求。

**InstanceBase类** - 业务实例基类，插件业务逻辑的实现载体。主要是管理实例的生命周期。

#### 请求处理流程

1. **初始化**：创建WebviewApiHandler，添加业务实例
2. **路由注册**：业务实例在Init方法中注册路由处理器
3. **请求匹配**：HTTP请求到达时，Router进行路径匹配
4. **处理器分发**：匹配成功后调用对应的HandlerBase处理器
5. **响应生成**：处理器执行业务逻辑，生成HTTP响应

### 使用示例

#### 业务类定义
```cpp
class UserApi : public InstanceBase {
public:
    bool Init(RouterBase &router) override {
        // 注册路由处理器
        return true;
    }
};
```

#### API处理器配置
```cpp
auto api_handler = new WebviewApiHandler("api");
auto user_api = api_handler->AddInstanceT<UserApi>();

// NOTE: 不同平台时机是不同的
webview->RegisterHandler(api_handler);
```

### 扩展机制

* **文档末尾的C++代码展示了处理器扩展的实现，包括自动参数解析、类型转换和便捷宏定义。这套扩展机制可以进一步简化Web API开发。**


#### 响应标准化

**统一响应格式**：
```cpp
template <typename _Ty> struct Response {
    int code = 200;           // HTTP状态码
    std::string msg = "OK";   // 响应消息  
    _Ty data;                 // 业务数据
};

template <typename _Ty> 
static wxString create_response(_Ty&& val) {
    using data_type = std::decay_t<_Ty>;
    Response<data_type> resp;
    resp.data = std::move(val);  
    auto str = object_to_json(resp);
    return wxString::FromUTF8(str.data(), str.size());  // UTF-8安全转换
}
```

**技术特点**：
- **类型推导**：自动推导实际数据类型
- **编码安全**：正确处理UTF-8编码转换

#### 智能参数转换引擎

支持两种参数模式：查询参数(URL)和JSON参数(Body)

**查询参数转换**：
```cpp
template <typename _Ty> 
static _Ty transform_query(const std::string& val) {
    using ret_type = std::decay_t<_Ty>;
    
    if constexpr (std::is_same_v<std::string, ret_type>) {
        return val;  // 字符串直接返回
    } 
    else if constexpr (std::is_arithmetic_v<ret_type>) {
        return boost::lexical_cast<ret_type>(val);  // 数值类型转换
    }
}
```

**JSON参数转换**：
```cpp
template <typename _Ty>
static void transform_json(_Ty&& ret, const boost::json::value& val) {
    using ret_type = std::decay_t<_Ty>;
    
    if constexpr (std::is_arithmetic_v<ret_type>) {
        ret = val.to_number<ret_type>();  // 数值类型转换
    }
    else if constexpr (std::is_same_v<std::string, ret_type>) {
        ret = val.as_string();  // 字符串转换
    }
}
```

#### 批量参数处理

**查询参数批量处理**：
```cpp
template <typename Tuple, std::size_t... I>
static void assign_query(Tuple& tuple, 
                        const std::vector<wxString>& names,
                        const query_type& list, 
                        std::index_sequence<I...>) {
    // 编译期展开参数包
    (assign_query_impl(std::get<I>(tuple), names[I], list), ...);
}
```

**JSON参数批量处理**：
```cpp
template <typename Tuple, std::size_t... I>
static void assign_json(Tuple& tuple,
                       const std::vector<wxString>& names,
                       const boost::json::value& json,
                       std::index_sequence<I...>) {
    // 编译期展开参数包
    (assign_json_impl(std::get<I>(tuple), names[I], json), ...);
}
```

#### 核心包装器 - WrapperMember

`WrapperMember`是将普通成员函数转换为Web API处理器的核心模板：

```cpp
template <bool query = false, typename Function, typename Self, typename ParamFunc>
HandlerBase* WrapperMember(const Function& func, Self* self,
                          const std::vector<wxString>& param_names,
                          const ParamFunc& get_str) {
    
    auto wrapper_func = [param_names, func, self, get_str]
                       (auto& req, auto res) -> bool {
        // 参数解析
        typename func_traits<Function>::bare_tuple_type args;
        auto body = get_str(req);
        detail::decode<query>(args, body, param_names);
        
        // 函数调用
        auto result = std::apply(func, 
            std::tuple_cat(std::make_tuple(self), args));
            
        // 响应生成
        wxString resBody = detail::create_response(std::move(result));
        
        // 响应设置
        res->SetContentType("application/json;charset=UTF-8");
        res->SetStatus(OK_200);
        res->Finish(wxSharedPtr<wxWebViewHandlerResponseData>(
            new detail::StringResponseData(resBody)));
            
        return true;
    };
    
    return new detail::FunctionHandler(wrapper_func);
}
```

**包装器工作流程**：
1. **参数解码**：根据模式(query/JSON)解析请求参数
2. **函数调用**：自动注入this指针，调用目标函数
3. **结果包装**：将返回值包装为标准JSON响应
4. **异常处理**：完善的错误处理

#### 便捷宏系统

**提供极简的路由注册语法：**

```cpp
#define MAKE_URI(module, func) "/api/v1/" #module "/" #func

#define ADD(method, type, module, func, ...)                                   \
    RegisterHandler(                                                           \
        RouterBase::BOOST_PP_CAT(Method, method), MAKE_URI(module, func),      \
        ::Anycubic::Plugins::SDK::handler::WrapperMember<                      \
            ::Anycubic::Plugins::SDK::handler::condition<                      \
                RouterBase::BOOST_PP_CAT(Method, method)>(true, false)>(       \
            &type::func, this, {__VA_ARGS__},                                  \
            ::Anycubic::Plugins::SDK::handler::condition<                      \
                RouterBase::BOOST_PP_CAT(Method, method)>(                     \
                ::Anycubic::Plugins::SDK::handler::GetQuery,                   \
                ::Anycubic::Plugins::SDK::handler::GetBody)))
```

### 完整使用示例

#### 1. 业务类定义
```cpp
class UserApi : public InstanceBase {
private:
    std::vector<User> users;
    
public:
    // 查询用户列表 - GET /api/v1/user/list
    std::vector<User> GetUserList() {
        return users;
    }
    
    // 创建用户 - POST /api/v1/user/create  
    User CreateUser(const std::string& name, int age) {
        User user{name, age};
        users.push_back(user);
        return user;
    }
    
    // 路由注册
    bool Init(RouterBase& router) override {
        router.ADD(GET, UserApi, user, list);           // GET /api/v1/user/list
        router.ADD(POST, UserApi, user, create, "name", "age"); // POST /api/v1/user/create
        return true;
    }
};
```

#### 2. API处理器配置
```cpp
// 创建API处理器
auto api_handler = new WebviewApiHandler("api");

// 添加业务实例
auto user_api = api_handler->AddInstanceT<UserApi>();

// 注册到WebView
webview->RegisterHandler(api_handler);
```

### 技术优势与创新点

#### 1. 编译期类型安全
- **模板元编程**：所有类型检查在编译期完成
- **错误预防**：类型不匹配在编译阶段即被发现

#### 2. 极简开发体验
- **自动参数映射**：无需手动解析请求参数
- **标准化响应**：统一的JSON响应格式
- **宏简化语法**：一行代码完成路由注册

#### 3. 生产就绪特性
- **异常安全**：完善的错误处理和日志记录
- **CORS支持**：内置跨域请求处理
- **UTF-8安全**：完整的编码转换支持


* **基本完整的代码实现**
  
```c++
namespace boost::json {
inline value external_value_from(const wxString &val) {
  boost::json::value val_ = boost::json::value(val.utf8_string());
  return val_;
}
} // namespace boost::json



namespace Anycubic::Plugins::SDK {

namespace handler {

namespace detail {

/**
 * @brief 函数处理器类
 * @details 实现HandlerBase接口，包装函数调用逻辑
 */
class FunctionHandler : public HandlerBase {
public:
  using CallType =
      std::function<bool(const wxWebViewHandlerRequest &request,
                         wxSharedPtr<wxWebViewHandlerResponse> resp)>;

public:
  /**
   * @brief 构造函数
   * @param call 要包装的函数调用器
   */
  FunctionHandler(CallType call) : func_(call) { assert(func_); }

  /**
   * @brief 重新加载处理器
   */
  void Reload(void) override {}

  /**
   * @brief 释放处理器资源
   */
  void Release(void) override { delete this; }

  /**
   * @brief 执行请求处理
   * @param request 请求对象
   * @param resp 响应对象
   * @return 处理是否成功
   */
  bool Execute(const wxWebViewHandlerRequest &request,
               wxSharedPtr<wxWebViewHandlerResponse> resp) const override {
    return func_(request, resp);
  }

private:
  CallType func_; ///< 包装的函数调用器
};

/**
 * @brief 字符串响应数据类
 * @details 将字符串数据包装为WebView响应流
 */
class StringResponseData : public wxWebViewHandlerResponseData {
public:
  /**
   * @brief 构造函数
   * @param data 要响应的字符串数据
   */
  StringResponseData(const wxString &data) : m_stream(data) {}

  /**
   * @brief 获取输入流
   * @return 字符串输入流指针
   */
  wxInputStream *GetStream() override { return &m_stream; }

private:
  wxStringInputStream m_stream; ///< 字符串输入流
};

/**
 * @brief 响应数据结构模板
 * @tparam _Ty 数据类型
 */
template <typename _Ty> struct Response {
  int code = 200;         ///< 响应状态码
  std::string msg = "OK"; ///< 响应消息
  _Ty data;               ///< 响应数据
};

/**
 * @brief 创建JSON响应字符串
 * @tparam _Ty 数据类型
 * @param val 要序列化的值
 * @return JSON格式的响应字符串
 */
template <typename _Ty> static wxString create_response(_Ty &&val) {
  using data_type = std::decay_t<_Ty>;
  Response<data_type> resp;
  resp.data = std::move(val);
  auto str = object_to_json(resp);
  return wxString::FromUTF8(str.data(), str.size());
}

/**
 * @brief 转换查询参数到指定类型
 * @tparam _Ty 目标类型
 * @param val 字符串值
 * @return 转换后的值
 */
template <typename _Ty> static _Ty transform_query(const std::string &val) {
  using ret_type = std::decay_t<_Ty>;
  if constexpr (std::is_same_v<std::string, ret_type>) {
    return val;
  } else if constexpr (std::is_same_v<wxString, ret_type>) {
    return wxString::FromUTF8(val.data(), val.size());
  } else if constexpr (std::is_arithmetic_v<ret_type>) {
    try {
      return boost::lexical_cast<ret_type>(val);
    } catch (...) {
      return ret_type(0);
    }
  } else {
    assert(false);
    return ret_type();
  }
}

/**
 * @brief 转换JSON值到指定类型
 * @tparam _Ty 目标类型
 * @param ret 接收转换结果的引用
 * @param val JSON值
 */
template <typename _Ty>
static void transform_json(_Ty &&ret, const boost::json::value &val) {
  using ret_type = std::decay_t<_Ty>;
  if constexpr (std::is_same_v<bool, ret_type>) {
    ret = val;
  } else if constexpr (std::is_floating_point_v<ret_type>) {
    ret = val;
  } else if constexpr (std::is_arithmetic_v<ret_type>) {
    ret = val.to_number<ret_type>();
  } else if constexpr (std::is_same_v<std::string, ret_type>) {
    ret = val.as_string();
  } else if constexpr (std::is_same_v<wxString, ret_type>) {
    auto str = val.as_string();
    ret = wxString::FromUTF8(str.data(), str.size());
  } else {
    assert(false);
    ret = L"77dk";
  }
}

/**
 * @brief 从查询参数列表分配值到指定变量
 * @tparam _Ty 变量类型
 * @param ret 接收值的变量引用
 * @param name 参数名
 * @param list 查询参数列表
 */
template <typename _Ty>
void assign_query_impl(_Ty &&ret, const wxString &name,
                       const query_type &list) {
  static_assert(!(std::is_const_v<_Ty>), "must be non-const");
  using value_t = std::decay_t<_Ty>;
  if (auto pair = list.find(name.ToStdString()); pair != list.end()) {
    ret = transform_query<value_t>(pair->second);
  }
}

/**
 * @brief 批量分配查询参数到元组
 * @tparam Tuple 元组类型
 * @tparam I 索引序列
 * @param tuple 目标元组
 * @param names 参数名列表
 * @param list 查询参数列表
 * @param seq 索引序列
 */
template <typename Tuple, std::size_t... I>
static void assign_query(Tuple &tuple, const std::vector<wxString> &names,
                         const query_type &list, std::index_sequence<I...>) {

  (assign_query_impl(std::get<I>(tuple), names[I], list), ...);
}

/**
 * @brief 从JSON对象分配值到指定变量
 * @tparam _Ty 变量类型
 * @param val 接收值的变量引用
 * @param name 字段名
 * @param json JSON对象
 */
template <typename _Ty>
void assign_json_impl(_Ty &&val, const wxString &name,
                      const boost::json::value &json) {
  static_assert(!(std::is_const_v<_Ty>), "must be non-const");
  assert(json.is_object());
  auto &obj = json.as_object();
  if (auto pair = obj.find(name.utf8_string()); pair != obj.end()) {
    transform_json(val, pair->value());
  }
}

/**
 * @brief 批量分配JSON字段到元组
 * @tparam Tuple 元组类型
 * @tparam I 索引序列
 * @param tuple 目标元组
 * @param names 字段名列表
 * @param json JSON对象
 * @param seq 索引序列
 */
template <typename Tuple, std::size_t... I>
static void assign_json(Tuple &tuple, const std::vector<wxString> &names,
                        const boost::json::value &json,
                        std::index_sequence<I...>) {
  (assign_json_impl(std::get<I>(tuple), names[I], json), ...);
}

/**
 * @brief 解码请求参数到元组
 * @tparam query 是否为查询参数模式
 * @tparam Args 参数类型列表
 * @param tuple 目标元组
 * @param str 参数字符串
 * @param names 参数名列表
 */
template <bool query, typename... Args>
static void decode(std::tuple<Args...> &tuple, const wxString &str,
                   const std::vector<wxString> &names) {
  assert(sizeof...(Args) <= names.size());
  if constexpr (query) {
    std::string jsonstr = wxURI::Unescape(str).utf8_string();
    auto args = parse_query(jsonstr);
    assign_query(tuple, names, args, std::index_sequence_for<Args...>{});
  } else {
    std::string jsonstr = str.utf8_string();
    auto json = boost::json::parse(jsonstr);
    assign_json(tuple, names, json, std::index_sequence_for<Args...>{});
  }
}

} // namespace detail

/**
 * @brief 包装成员函数为WebView处理器
 * @tparam query 是否为查询参数模式
 * @tparam Function 函数类型
 * @tparam Self 类类型
 * @tparam ParamFunc 参数获取函数类型
 * @param func 要包装的成员函数
 * @param self 类实例指针
 * @param param_names 参数名列表
 * @param get_str 参数获取函数
 * @return 包装后的处理器指针
 */
template <bool query = false, typename Function, typename Self,
          typename ParamFunc>
HandlerBase *WrapperMember(const Function &func, Self *self,
                           const std::vector<wxString> &param_names,
                           const ParamFunc &get_str) {
  typedef traits::function_traits<Function> func_traits;
  auto wrapper_func = [param_names, func, self, get_str](auto &req,
                                                         auto res) -> bool {
    typename func_traits::bare_tuple_type args;
    using ret_type = typename func_traits::return_type;
    wxString resBody = R"({"code":500,"msg":"Internal Server Error"})";
    int32_t code = InternalServerError_500;
    try {
      auto body = get_str(req);
      detail::decode<query>(args, body, param_names);
      resBody = detail::create_response(
          std::apply(func, std::tuple_cat(std::make_tuple(self), args)));
      code = OK_200;
    } catch (...) {
      LOG_ERROR("WrapperMember: {}", typeid(decltype(self)).name());
    }
    res->SetContentType("application/json;charset=UTF-8");

    res->SetStatus(code);
    res->Finish(wxSharedPtr<wxWebViewHandlerResponseData>(
        new detail::StringResponseData(resBody)));
    return true;
  };
  return new detail::FunctionHandler(wrapper_func);
}

inline wxString GetQuery(const wxWebViewHandlerRequest &req) {
  wxURI uri(req.GetRawURI());
  return uri.GetQuery();
}
inline wxString GetBody(const wxWebViewHandlerRequest &req) {
  return req.GetDataString();
}

template <RouterBase::METHOD_TYPE method, typename _Ty>
constexpr inline _Ty condition(_Ty left, _Ty right) {
  if constexpr (method == RouterBase::MethodGET) {
    return left;
  } else {
    return right;
  }
}
} // namespace handler
} // namespace Anycubic::Plugins::SDK

#define MAKE_URI(module, func) "/api/v1/" #module "/" #func

#define ADD(method, type, module, func, ...)                                   \
  RegisterHandler(                                                             \
      RouterBase::BOOST_PP_CAT(Method, method), MAKE_URI(module, func),        \
      ::Anycubic::Plugins::SDK::handler::WrapperMember<                        \
          ::Anycubic::Plugins::SDK::handler::condition<                        \
              RouterBase::BOOST_PP_CAT(Method, method)>(true, false)>(         \
          &type::func, this, {__VA_ARGS__},                                    \
          ::Anycubic::Plugins::SDK::handler::condition<                        \
              RouterBase::BOOST_PP_CAT(Method, method)>(                       \
              ::Anycubic::Plugins::SDK::handler::GetQuery,                     \
              ::Anycubic::Plugins::SDK::handler::GetBody)))
```