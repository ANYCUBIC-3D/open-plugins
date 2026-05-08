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
#include <windows.h>
#include <intrin.h>

#include "instrument_export.hxx"



#include <mutex>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>


#define FUNC_ATTR __declspec(noinline)

extern "C" {

INSTRUMENT_API
void _cdecl _penter(void) {
  // 获取返回地址（调用者的地址）
  void *faddress = _ReturnAddress();
}

INSTRUMENT_API
void _cdecl _pexit(void) {
  // 获取返回地址
  void *faddress = _ReturnAddress();
}
}
