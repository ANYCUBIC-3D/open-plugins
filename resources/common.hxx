
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