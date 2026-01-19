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

#include <stddef.h>
#include <string>

#define MD5LEN 16

/**
 * @brief 计算字符串的MD5值
 *
 * @param src 输入字符串指针
 * @param srcLen 输入字符串大小
 * @param md5 输出MD5值指针
 */
extern "C" void md5Sum(const char *src, size_t srcLen, char md5[MD5LEN]);

/**
 * @brief 计算文件的MD5值
 *
 * @param filename 文件名指针
 * @param md5 输出MD5值指针
 * @return true 成功
 * @return false 失败
 */
extern "C" bool md5File(const char *filename, char md5[MD5LEN]);
struct evp_md_ctx_st;

/**
 * @brief 计算MD5值的上下文
 *
 */
struct cMd5 {
  /**
   * @brief 写入数据
   *
   * @param src 输入数据指针
   * @param srcLen 输入数据大小
   */
  void write(const char *src, size_t srcLen);

  /**
   * @brief 计算MD5值
   *
   * @param md5 输出MD5值指针
   */
  void sum(char md5[MD5LEN]);
  cMd5(void);
  ~cMd5(void);

private:
  evp_md_ctx_st *evpCtx = nullptr;
};
