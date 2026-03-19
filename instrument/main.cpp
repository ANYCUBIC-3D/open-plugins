
#include <easy_log/log.hxx>

#ifdef NDEBUG
#define CLOUD_API_FLAG 0
#else
#define CLOUD_API_FLAG                                                         \
  (FLAG_HTTP_DISABLE_COMPRESSIONS | FLAG_LOG_HTTPVERBOSE | FLAG_LOG_HTTP_TEXT)
#endif

#ifdef _WIN32
#include <windows.h>
BOOL WINAPI DllMain(HINSTANCE hinstDLL, // handle to DLL module
                    DWORD fdwReason,    // reason for calling function
                    LPVOID lpvReserved) // reserved
{
  // Perform actions based on the reason for calling.
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    // Initialize once for each new process.
    REGISTER_LOGGER(true);
    break;

  case DLL_PROCESS_DETACH:
    if (lpvReserved != nullptr) {
      break; // do not do cleanup if process termination scenario
    }
    UNREGISTER_LOGGER();
    break;
  }
  return TRUE; // Successful DLL_PROCESS_ATTACH.
}
#elif __linux__ || __APPLE__
// Linux/macOS模块初始化属性
__attribute__((constructor)) static void module_init() {
  REGISTER_LOGGER(true);
}
__attribute__((destructor)) static void module_cleanup() {
  UNREGISTER_LOGGER();
}
#else
#error "Unsupported platform"
#endif