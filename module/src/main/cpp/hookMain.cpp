#include <unistd.h>
#include <string>
#include <thread>
#include "il2cppHook.hpp"
#include "main.hpp"
#include "shadowhook.h"
#include "xdl.h"

namespace hookMain {
  static std::thread hookThread_;
  static std::thread il2cppThread_;
  static void *linkerStub   = nullptr;
  static void *il2cppHandle = nullptr;
  static void **dlopenO     = nullptr;

  _LIBCPP_VISIBILITY("hidden")
  static void *dlopenX(const char *, int, const void *, const void *);
  _LIBCPP_VISIBILITY("hidden")
  static void hookThreadEntryPoint();

  static void *dlopenX(
    const char *name, int flags, const void *extinfo, const void *caller_addr
  ) {
    void *handle = reinterpret_cast<decltype(&dlopenX)>(dlopenO)(name, flags, extinfo, caller_addr);
    if (std::string(name).ends_with("libil2cpp.so")) {
      il2cppHandle = handle;
      shadowhook_unhook(linkerStub);
      il2cppThread_ = std::thread(il2cppHook::HookIl2cpp, il2cppHandle);
    }
    return handle;
  }

  static void hookThreadEntryPoint() {
    LOGD("hook thread tid: %d", gettid());
    int suc = shadowhook_init(SHADOWHOOK_MODE_SHARED, true);
    if (suc != 0) {
      LOGE("failed to initialize shadowhook, err code: %d", suc);
      LOGE("%s", shadowhook_to_errmsg(suc));
      return;
    }
    linkerStub = shadowhook_hook_sym_name(
      "linker64",
      "__dl__Z9do_dlopenPKciPK17android_dlextinfoPKv",
      (void *)dlopenX,
      (void **)&dlopenO
    );
    if (linkerStub == nullptr) {
      int errCode = shadowhook_get_errno();
      LOGE("hook linker failed, err code: %d", errCode);
      LOGE("%s", shadowhook_to_errmsg(errCode));
      return;
    }
  }

  void Entrypoint() {
    LOGD("create hook thread");
    hookThread_ = std::thread(hookThreadEntryPoint);
  }
}
