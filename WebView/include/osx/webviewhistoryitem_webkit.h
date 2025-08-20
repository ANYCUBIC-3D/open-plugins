/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/osx/webviewhistoryitem.h
// Purpose:     wxWebViewHistoryItem header for OSX
// Author:      Steven Lamerton
// Copyright:   (c) 2011 Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_WEBVIEWHISTORYITEM_H_
#define _WX_OSX_WEBVIEWHISTORYITEM_H_
#include "../setup.h"
#include "wx/defs.h"

#if USE_WEBVIEW && USE_WEBVIEW_WEBKIT && defined(__WXOSX__)

#include "wx/osx/core/objcid.h"

class ACWEBVIEW_API wxWebViewHistoryItem
{
public:
    wxWebViewHistoryItem(const wxString& url, const wxString& title) :
                     m_url(url), m_title(title) {}
    wxString GetUrl() { return m_url; }
    wxString GetTitle() { return m_title; }

    friend class wxWebViewWebKit;

private:
    wxString m_url, m_title;
    wxObjCID m_histItem;
};

#endif // USE_WEBVIEW && USE_WEBVIEW_WEBKIT

#endif // _WX_OSX_WEBVIEWHISTORYITEM_H_
