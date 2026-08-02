#include <fstream>
#include <string>
#include <format>
#include <mutex>

#include "main.hpp"
#include "utils.hpp"

namespace utils {
  static std::mutex fileMutex;

  int WriteText(const std::string& text, const std::string& file) {
    std::lock_guard<std::mutex> guard(fileMutex);
    std::ofstream ofs(file, std::ios_base::app);
    if (!ofs.is_open()) {
      LOGE("cannot open %s", file.c_str());
      return 1;
    }
    ofs << text;
    ofs.flush();
    ofs.close();
    return 0;
  }

  std::string GetCsByteArrayString(URT::Array<URT::Byte>* csByteArray) {
    std::vector<URT::Byte> byteVec = csByteArray->ToVector();
    std::string hexStr;
    hexStr.reserve(csByteArray->max_length * 2);
    for (URT::Byte byt : byteVec) {
      hexStr += std::format("{:02X}", byt);
    }
    return hexStr;
  }
}
