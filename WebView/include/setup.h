#pragma once

#define USE_WEBVIEW 1
#ifdef __WXMSW__
#define USE_WEBVIEW_EDGE_STATIC 1
#endif // __WXMSW__
#define USE_WEBVIEW_EDGE 0
#define USE_WEBVIEW_WEBKIT 1
#define USE_WEBVIEW_CHROMIUM 0
#define USE_WEBVIEW_WEBKIT2 0

#include <memory>
