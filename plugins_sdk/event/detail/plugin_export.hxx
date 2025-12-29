#pragma once

#if defined(_WIN32)
#if defined(ACPLUGIN_API_BUILD)
#define ACPLUGIN_API __declspec(dllexport)
#else
#define ACPLUGIN_API __declspec(dllimport)
#endif
#else
#define ACPLUGIN_API
#endif
