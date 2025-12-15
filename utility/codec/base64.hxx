#pragma once
#include <string>

/**
 * @brief base64 标准编码实现
 *
 * @param src 输入数数据
 * @return std::string 输出base64编码字符串
 */
std::string base64Encode(const std::string &src);

/**
 * @brief base64 标准解码实现
 *
 * @param src base64编码字符串
 * @return std::string 输出解码后的字符串
 */
std::string base64Decode(const std::string &src);