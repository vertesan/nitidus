#include "main.hpp"
#include "URH.hpp"
#include "shadowhook.h"
#include "types.hpp"

namespace demo {
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
      LOGD("hook installed: %s, addr: %lu", #NAME, reinterpret_cast<unsigned long>(ADDR)); \
      hookStubs.emplace_back(stub);                                                        \
    }                                                                                      \
  } else {                                                                                 \
    LOGE("function address for %s is null, abort adding hook", #NAME);                     \
  }

#define ADD_HOOK_M(NAME, assemblyName, spaceName, className, methodName, ...)                        \
  auto method##NAME = URH::GetMethod(assemblyName, spaceName, className, methodName, {__VA_ARGS__}); \
  ADD_HOOK(NAME, method##NAME->function)

  // ============================================

  // define a hook
  DEF_HOOK(resolveAllDeps, int, (URT::Array<types::EntryExample> * entries, void *revMap, const URT::MethodInfo *method)) {
    LOGD("invoking method: %s", "resolveAllDeps");
    int ret = resolveAllDepsOrig(entries, revMap, method);

    LOGD("entry size: %lu", entries->max_length);
    auto entry1 = entries->At(0);
    LOGD("entry1 at %lX", reinterpret_cast<unsigned long>(&entry1));
    LOGD("priority: %d", entry1.Priority);
    LOGD("size: %lu", entry1.Size);

    return ret;
  }

  DEF_HOOK(fastAesDecrypt, int32_t, (void* self, void* cipher, int32_t offset, int32_t length, int32_t mode, int32_t padding, int32_t keySize, URT::Array<URT::Byte>* key, URT::Array<URT::Byte>* iv, URT::Array<URT::Byte>* output)) {
    LOGD("invoking method: %s", "fastAesDecrypt");
    LOGI("=== Invoking FastAES Decrypt ===");
    LOGI("addr self: 0x%lX", (unsigned long)self);
    LOGI("cipher: 0x%lX", (unsigned long)cipher);
    LOGI("offset: %d", offset);
    LOGI("length: %d", length);
    LOGI("mode: %d", mode);
    LOGI("padding: %d", padding);
    LOGI("keySize: %d", keySize);
    LOGI("addr key: 0x%lX", (unsigned long)key);
    LOGI("addr iv: 0x%lX", (unsigned long)iv);
    LOGI("addr output: 0x%lX", (unsigned long)output);
    LOGI("key is: 0x%s", utils::GetCsByteArrayString(key).c_str());
    LOGI("iv  is: 0x%s", utils::GetCsByteArrayString(iv).c_str());
    auto ret = fastAesDecryptOrig(self, cipher, offset, length, mode, padding, keySize, key, iv, output);
    LOGI("result: 0x%X", ret);
    return ret;
  }

  // add a defined hook
  void addHook() {
    ADD_HOOK_M(resolveAllDeps, "Core.dll", "Hailstorm", "Catalog", "ResolveAllDeps")
    // or
    auto catalogClass = URH::GetClass("Core.dll", "Hailstorm", "Catalog");
    auto method       = catalogClass->Get<UR::Method>("ResolveAllDeps");
    ADD_HOOK(resolveAllDeps, method->function)
    // another complex example
    auto fastAesClass = URH::GetClass("FastAES.dll", "", "FastAES");
    auto method       = fastAesClass->Get<UR::Method>("Decrypt", std::vector<std::string>{"System.ReadOnlySpan<System.Byte>", "System.Int32", "System.Int32", "System.Security.Cryptography.CipherMode", "System.Security.Cryptography.PaddingMode", "System.Int32", "System.Byte[]", "System.Byte[]", "System.Byte[]"});
    for (auto arg : method->args) {
      LOGD("FastAES.Decrypt arg name: %s: %s", arg->name.c_str(), arg->pType->name.c_str());
    }
    ADD_HOOK(fastAesDecrypt, method->function)
  }

  void getNestedClass() {
    const auto pClass = URH::GetClass("Core.dll", "Hailstorm", "Catalog");
    auto entryClass   = URH::GetNestedClass(pClass->address, "Entry");
    // or
    auto entryClass2 = URH::GetNestedClass("Core.dll", "Hailstorm", "Catalog", "Entry");
  }

  void getAMethod(UR::Class *urClass) {
    auto method  = urClass->Get<UR::Method>("FromSignature");
    auto method2 = urClass->Get<UR::Method>("FromSignature", {"*", "System.Int32", "System.String"});
    // invoke method
    auto str = method->Invoke<URT::String *>();
    method2->Invoke<int>(114, 514, "114514");
  }

}
