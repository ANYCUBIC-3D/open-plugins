
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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct PackageInfo {
  const char *name;      ///< Package name
  const char *md5;       ///< md5 of package data
  const char *password;  ///< Password for package
  const char *prefixDir; ///< Root directory of package data
  const uint8_t *data;   ///< Package data
  uint32_t size;         ///< Package size in bytes
  uint32_t type;         ///< Package type
};
static_assert(sizeof(PackageInfo) == 48, "PackageInfo size mismatch");

#ifdef __cplusplus
}
#endif // __cplusplus