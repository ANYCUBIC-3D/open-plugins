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