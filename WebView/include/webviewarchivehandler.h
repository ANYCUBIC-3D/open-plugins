/////////////////////////////////////////////////////////////////////////////
// Name:        webviewarchivehandler.h
// Purpose:     Custom webview handler to allow archive browsing
// Author:      Steven Lamerton
// Copyright:   (c) 2011 Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBVIEW_FILE_HANDLER_H_
#define _WX_WEBVIEW_FILE_HANDLER_H_

#include "setup.h"

#if USE_WEBVIEW

class wxFSFile;
class wxFileSystem;

#include "webview.h"

//Loads from uris such as scheme:///C:/example/example.html or archives such as
//scheme:///C:/example/example.zip;protocol=zip/example.html

class WXDLLIMPEXP_WEBVIEW wxWebViewArchiveHandler : public wxWebViewHandler
{
public:
    wxWebViewArchiveHandler(const wxString& scheme);
    virtual ~wxWebViewArchiveHandler();
    virtual wxFSFile* GetFile(const wxString &uri) override;
private:
    wxFileSystem* m_fileSystem;
};

#endif // USE_WEBVIEW

#endif // _WX_WEBVIEW_FILE_HANDLER_H_
