///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/webview.h
// Purpose:     wxWebView implementation classes
// Author:      Tobias Taschner
// Created:     2023-03-16
// Copyright:   (c) 2023 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_WEBVIEW_H_
#define _WX_PRIVATE_WEBVIEW_H_
#ifdef DEVTOOL_ENALBED
#include "ACWebView_export.h"
#else
#include "webview/ACWebView_export.h"
#endif
#ifndef _
#define _(x) x
#endif
class ACWEBVIEW_API wxWebViewConfigurationImpl
{
public:
    virtual ~wxWebViewConfigurationImpl() = default;
    virtual void* GetNativeConfiguration() const { return nullptr; }
    virtual void SetDataPath(const wxString& WXUNUSED(path)) {}
    virtual wxString GetDataPath() const { return wxString{}; }
    virtual bool EnablePersistentStorage(bool WXUNUSED(enable)) { return false; }
    virtual bool ClearCookies(void) { return false; }
};

#endif // _WX_PRIVATE_WEBVIEW_H_
