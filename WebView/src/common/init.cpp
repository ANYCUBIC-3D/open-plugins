#include "init.h"
#include "private/init.h"

#include <vector>


// ----------------------------------------------------------------------------
// Entry hook support
// ----------------------------------------------------------------------------

namespace
{

// All registered entry hooks.
std::vector<EntryHook>& GetEntryHooks()
{
    static std::vector<EntryHook> s_entryHooks;
    return s_entryHooks;
}

} // anonymous namespace

void AddEntryHook(EntryHook hook)
{
    // Order doesn't really matter, we suppose that we're never going to have
    // more than one hook that would apply to the same program run.
    GetEntryHooks().push_back(hook);
}




int init_webview(int argc, char* argv[])
{    
    // Check if we have any hooks that can hijack the application execution.
    for ( auto& hook : GetEntryHooks() )
    {
        const int rc = (*hook)(argc,argv);
        if ( rc != -1 )
           {
             return rc;
           }
    }
    return 0;
}
