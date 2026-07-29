#include <unistd.h>
#include <string>
#include <thread>
#include "il2cppHook.hpp"
#include "main.hpp"
#include "shadowhook.h"
#include "xdl.h"
#include "URH.hpp"

namespace hookMain {
  static std::thread hookThread_;
  static std::thread il2cppThread_;
  static std::thread initThread_;
  static void* il2cppHandle = nullptr;
  static void* il2cppInitStub;
  static int (*il2cppInitOrig)(const char*);

  _LIBCPP_VISIBILITY("hidden")
  static void hookThreadEntryPoint();
  _LIBCPP_VISIBILITY("hidden")
  static void il2cppInitImpl();

  static void il2cppInitImpl() {
    il2cppHandle = shadowhook_dlopen("libil2cpp.so");
    if (il2cppHandle != nullptr) {
      LOGD("dlopen libil2cpp.so handle at 0x%lX", (unsigned long)il2cppHandle);
      UR::Init(il2cppHandle, UR::Mode::Il2Cpp);
//      UR::ThreadAttach();
      LOGD("il2cpp initialized, pid: %d, tid: %d", getpid(), gettid());
      il2cppHook::il2cppHookEntrypoint(il2cppHandle);
    } else {
      LOGE("failed to dlopen libil2cpp.so: dlopen returned null");
    }
  }

  static int il2cppInitProxy(const char* domain_name) {
    int ret = il2cppInitOrig(domain_name);
    if (NTD_IL2CPP_ASYNC_INIT) {
      initThread_ = std::thread(il2cppInitImpl);
    } else {
      il2cppInitImpl();
    }
    return ret;
  }

  static void libil2cppHookedCallback(int error_number, const char* lib_name, const char* sym_name, void* sym_addr, void* new_addr, void* orig_addr, void* arg) {
    const char* error_msg = shadowhook_to_errmsg(error_number);
    LOGD("hook finished: %s, %s, %d - %s", lib_name, sym_name, error_number, error_msg);
  }

  static void hookThreadEntryPoint() {
    LOGD("hook thread tid: %d", gettid());
    int suc = shadowhook_init(SHADOWHOOK_MODE_UNIQUE, NTD_SHADOWHOOK_DEBUG_LOG);
    if (suc != 0) {
      LOGE("failed to initialize shadowhook, err code: %d", suc);
      LOGE("%s", shadowhook_to_errmsg(suc));
      return;
    }

    il2cppInitStub = shadowhook_hook_sym_name_callback(
      "libil2cpp.so",
      "il2cpp_init",
      reinterpret_cast<void*>(il2cppInitProxy),
      reinterpret_cast<void**>(&il2cppInitOrig),
      libil2cppHookedCallback,
      nullptr
    );
    int error_num = shadowhook_get_errno();
    if (il2cppInitStub == nullptr) {
      const char* error_msg = shadowhook_to_errmsg(error_num);
      LOGE("hook failed: %p, %d - %s", il2cppInitStub, error_num, error_msg);
      return;
    } else if (error_num == 1) {
      LOGD("hook pending");
    }
  }

  void Entrypoint() {
    LOGD("create hook thread");
    hookThread_ = std::thread(hookThreadEntryPoint);
  }
}
