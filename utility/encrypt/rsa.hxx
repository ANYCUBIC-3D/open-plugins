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
 * @brief 打开文件公钥证书
 *
 * @param ca 证书路径
 * @return struct evp_pkey_st*
 * @note 失败返回nullptr
 */
struct evp_pkey_st *openPublicKeyFile(const char *ca);

/**
 * @brief 打开内存公钥证书
 *
 * @param data 证书数据缓冲区
 * @param bytes 证书长工
 * @return struct evp_pkey_st*
 * @note 失败返回nullptr
 */
struct evp_pkey_st *openPublicKeyMem(const char *data, size_t bytes);

/**
 * @brief 证书内存释放
 *
 */
void pkey_free(struct evp_pkey_st *);

/**
 * @brief 公钥加密实现
 *
 * @param pkey 公钥
 * @param src  加密源数据
 * @return std::string 加密后数据
 */
std::string encryptPubkey(struct evp_pkey_st *pkey, const std::string &src);
