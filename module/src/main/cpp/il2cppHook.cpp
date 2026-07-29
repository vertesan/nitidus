#include <dlfcn.h>
#include <unistd.h>
#include <string>
#include <thread>
#include "URH.hpp"
#include "main.hpp"
#include "shadowhook.h"
#include "types.hpp"

namespace il2cppHook {
  using namespace types;

  static void *il2cppHandle_ = nullptr;
  static std::vector<void *> hookStubs = {};

#define DEF_HOOK(N, R, A)         \
  static R(*N##Orig) A = nullptr; \
  static R N##Hook A

#define ADD_HOOK(NAME, ADDR)                                                               \
  if (ADDR) {                                                                              \
    auto stub = shadowhook_hook_func_addr(                                                 \
      reinterpret_cast<void *>(ADDR),                                                      \
      reinterpret_cast<void *>(NAME##Hook),                                                \
      reinterpret_cast<void **>(&NAME##Orig)                                               \
    );                                                                                     \
    if (!stub) {                                                                           \
      int errCode        = shadowhook_get_errno();                                         \
      const char *errMsg = shadowhook_to_errmsg(errCode);                                  \
      LOGE("install hook %s failed, errCode: %d, errMsg: %s", #NAME, errCode, errMsg);     \
    } else {                                                                               \
      LOGD("hook installed: %s, addr: %lX", #NAME, reinterpret_cast<unsigned long>(ADDR)); \
      hookStubs.emplace_back(stub);                                                        \
    }                                                                                      \
  } else {                                                                                 \
    LOGE("function address for %s is null, abort adding hook", #NAME);                     \
  }

#define ADD_HOOK_M(NAME, assemblyName, spaceName, className, methodName, ...)                        \
  auto method##NAME = URH::GetMethod(assemblyName, spaceName, className, methodName, {__VA_ARGS__}); \
  ADD_HOOK(NAME, method##NAME->function)

  // ===================================================================
  // HINT: define hooks here

  void il2cppHookEntrypoint(void* il2cppHandle) {
    il2cppHandle_ = il2cppHandle;
    // =================================================================
    // HINT: add hooks here
  }
}
