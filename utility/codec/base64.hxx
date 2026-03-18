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