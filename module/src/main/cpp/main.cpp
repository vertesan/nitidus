#include <unistd.h>
#include <cstring>
#include "main.hpp"
#include "hookMain.hpp"
#include "zygisk.hpp"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

class HookModule : public zygisk::ModuleBase {
 public:
  void onLoad(Api *api, JNIEnv *env) override {
    this->api = api;
    this->env = env;
  }

  void postAppSpecialize(const AppSpecializeArgs *args) override {
    const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
    if (strcmp(process, NTD_TARGET_APP) == 0) {
      LOGD("get process nice name: %s, pid: %d", process, getpid());
      hookMain::Entrypoint();
    }
    env->ReleaseStringUTFChars(args->nice_name, process);
  }

 private:
  Api *api;
  JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(HookModule)
