#pragma once


// Type of the hook function, see AddEntryHook(). If this function returns
// a value different from -1, the process exits using it as error code.
using EntryHook = int (*)(int,char**);
void AddEntryHook(EntryHook hook);
