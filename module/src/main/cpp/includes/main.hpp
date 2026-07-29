#pragma once

#include <android/log.h>

// HINT: replace app id here
#define NTD_TARGET_APP "com.company.game"
// logcat tag for this module
#define NTD_MODULE_NAME "nitidus"
// if false, block game thread while il2cpp is still under initialization
#define NTD_IL2CPP_ASYNC_INIT true
// enable/disable shadowhook debug log
#define NTD_SHADOWHOOK_DEBUG_LOG true

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, NTD_MODULE_NAME, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, NTD_MODULE_NAME, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, NTD_MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, NTD_MODULE_NAME, __VA_ARGS__)
