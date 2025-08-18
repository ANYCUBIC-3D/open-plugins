#pragma once

#ifdef __APPLE__
#define PLUGIN_EXT wxASCII_STR(".dylib")
#elif __linux__
#define PLUGIN_EXT wxASCII_STR(".so")
#elif _WIN32
#define PLUGIN_EXT wxASCII_STR(".dll")
#else
#error "unknown platform"
#endif