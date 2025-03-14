#include <dlfcn.h>
#include <unistd.h>
#include <string>
#include <thread>
#include "URH.hpp"
#include "main.hpp"
#include "shadowhook.h"
#include "types.hpp"

#define ASYNC_INIT 1

namespace il2cppHook {
  using namespace types;

  static void *il2cppHandle = nullptr;
  static std::thread initThread;
  static std::vector<void *> hookStubs = {};

  static void initURHook();

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

  static void initURHook() {
    UR::Init(il2cppHandle, UR::Mode::Il2Cpp);
    UR::ThreadAttach();
    LOGD("il2cpp thread attached, tid: %d", gettid());

    // =================================================================
    // HINT: add hooks here
  }

  DEF_HOOK(il2cppInit, int, (const char *domain_name)) {
    int ret = il2cppInitOrig(domain_name);
    if (ASYNC_INIT) {
      initThread = std::thread(initURHook);
    } else {
      initURHook();
    }
    return ret;
  }

  void HookIl2cpp(void *handle) {
    il2cppHandle     = handle;
    auto pIl2cppInit = reinterpret_cast<int (*)(const char *)>(dlsym(il2cppHandle, "il2cpp_init"));
    ADD_HOOK(il2cppInit, pIl2cppInit)
  }
}
