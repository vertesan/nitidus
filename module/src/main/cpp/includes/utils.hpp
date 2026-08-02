#pragma once

#include <string>
#include "URH.hpp"

#define TARGET_FILE_PATH "/sdcard/Download/nitidus_hook.txt"

namespace utils {
  int WriteText(const std::string& text, const std::string& file = TARGET_FILE_PATH);
  std::string GetCsByteArrayString(URT::Array<URT::Byte>* csByteArray);
}
