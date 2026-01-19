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

#ifdef __APPLE__
#define PLUGIN_EXT wxASCII_STR(".dylib")
#define PLUGIN_PREFIX wxASCII_STR("libplugin_")
#elif __linux__
#define PLUGIN_EXT wxASCII_STR(".so")
#define PLUGIN_PREFIX wxASCII_STR("libplugin_")
#elif _WIN32
#define PLUGIN_EXT wxASCII_STR(".dll")
#define PLUGIN_PREFIX wxASCII_STR("plugin_")
#else
#error "unknown platform"
#endif