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
}
