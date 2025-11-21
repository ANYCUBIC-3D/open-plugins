#pragma once

#include <map>
#include <string>
#include <tuple>
#include <type_traits>

#include "lexical_cast.hxx"

////////////////////////////////////////////////////////////////////////////
/*                    实现url请求参数生成处理                               */
////////////////////////////////////////////////////////////////////////////
namespace Anycubic::utility {
/**
 * @brief query参数同名多个参数！！
 */
typedef std::multimap<std::string, std::string> query_type;

template <typename kType, typename vType>
void append_(query_type &query, kType &&key, vType &&value) {
  if constexpr (std::is_pointer_v<std::remove_reference_t<vType>>) {
    if (value == nullptr) {
      return;
    }
  }

  query.emplace(lexical_cast<std::string>(key),
                lexical_cast<std::string>(value));
}

void append(query_type &);

/**
 * @brief  添加键值
 * @tparam kType  key type
 * @tparam vType value type
 * @tparam ...Args key_value 其他参数类型
 * @param val 参数列表
 * @param key key
 * @param value value
 * @param ...args other key_value
 */
template <typename kType, typename vType, typename... Args>
void append(query_type &val, kType &&key, vType &&value, Args &&...args) {
  append_(val, std::forward<kType>(key), std::forward<vType>(value));
  append(val, std::forward<Args>(args)...);
}

/**
 * @brief  生成查询参数列表
 * @param query	请询参数列表
 * @return 规范的查询参数字串
 */
std::string make_query(const query_type &query);

template <typename... Args> query_type make_query_value(Args &&...args) {
  query_type query;
  append(query, std::forward<Args>(args)...);
  return std::move(query);
}
/**
 * @brief 生成查询参数列表——帮助实现
 *		base on make_query
 * @tparam ...Args  key_value 参数类型
 * @param ...args key_value参数
 * @return 规范的查询参数字串
 */
template <typename... Args>
std::enable_if_t<sizeof...(Args) % 2 == 0, std::string>
make_query(Args &&...args) {
  query_type query = make_query_value(std::forward<Args>(args)...);
  return make_query(query);
}

/**
 * @brief  分析请求参数
 *
 * @param query
 * @return query_type
 */
query_type parse_query(const std::string &query);
} // namespace Anycubic::utility