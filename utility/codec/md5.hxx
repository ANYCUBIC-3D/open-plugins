#pragma once

#include <stddef.h>
#include <string>

#define MD5LEN 16
extern "C" void md5Sum(const char *src, size_t srcLen, char md5[MD5LEN]);
extern "C" bool md5File(const char *filename, char md5[MD5LEN]);
struct evp_md_ctx_st;
struct cMd5 {

  void write(const char *src, size_t srcLen);
  void sum(char md5[MD5LEN]);
  cMd5(void);
  ~cMd5(void);

private:
  evp_md_ctx_st *evpCtx = nullptr;
};
