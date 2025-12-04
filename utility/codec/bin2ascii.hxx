#pragma once

#include <stdint.h>

#define LOOKUP_CAPS "0123456789ABCDEF"
#define LOOKUP_LOWER "0123456789abcdef"

/**
 * @brief 十六进制字符串转换为二进制数据
 *
 * @param _des 输出二进制数据指针
 * @param pstr 输入十六进制字符串指针
 * @param length 输入十六进制字符串大小
 */
void hex2bin(char *_des, const char *pstr, uint32_t length);

/**
 * @brief 二进制数据转换为十六进制字符串
 *
 * @param _des 输出十六进制字符串指针
 * @param pstr 输入二进制数据指针
 * @param length 输入二进制数据大小
 * @param lookup 十六进制字符集指针
 */
void bin2hex(char *_des, const char *pstr, uint32_t length,
             const char *lookup = LOOKUP_CAPS);
