/////////////////////////////////////////////////////////////////////////////
// Name:        webviewfshandler.h
// Purpose:     Custom webview handler for virtual file system
// Author:      Nick Matthews
// Copyright:   (c) 2012 Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// Based on webviewarchivehandler.h file by Steven Lamerton

#ifndef _WX_WEBVIEW_FS_HANDLER_H_
#define _WX_WEBVIEW_FS_HANDLER_H_

#include "setup.h"

#if USE_WEBVIEW

class wxFSFile;
class wxFileSystem;

#include "webview.h"

//Loads from uris such as scheme:example.html

class ACWEBVIEW_API wxWebViewFSHandler : public wxWebViewHandler
{
public:
    wxWebViewFSHandler(const wxString& scheme);
    virtual ~wxWebViewFSHandler();
    virtual wxFSFile* GetFile(const wxString &uri) override;
private:
    wxFileSystem* m_fileSystem;
};

#endif // USE_WEBVIEW

#endif // _WX_WEBVIEW_FS_HANDLER_H_
