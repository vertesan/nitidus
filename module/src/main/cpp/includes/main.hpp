#pragma once

#include <android/log.h>

// HINT: replace app id here
#define NTD_TARGET_APP "com.company.game"
#define NTD_MODULE_NAME "nitidus"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, NTD_MODULE_NAME, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, NTD_MODULE_NAME, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, NTD_MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, NTD_MODULE_NAME, __VA_ARGS__)
