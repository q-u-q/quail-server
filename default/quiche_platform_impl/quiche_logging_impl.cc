
#include "quiche_platform_impl/quiche_logging_impl.h"

#include <chrono>
#include <iomanip>

namespace quiche {

std::string getNowTime() {
  auto now = std::chrono::system_clock::now();

  // get number of milliseconds for the current second
  // (remainder after division into seconds)
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) %
            1000;

  // convert to std::time_t in order to convert to std::tm (broken time)
  auto timer = std::chrono::system_clock::to_time_t(now);

  // convert to broken time
  std::tm bt = *std::localtime(&timer);

  std::ostringstream oss;

  oss << std::put_time(&bt, "%Y-%m-%d %H:%M:%S");  // HH:MM:SS
  oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

  return oss.str();
}

MyLog::~MyLog() {
  if (level_ < INFO) {
    return;
  }

  std::string p1("\x1B[34mLog(");
  if (level_ == VERBOSE) {
    p1 = "\x1B[34m[ VERBO] ";
  } else if (level_ == DEBUG) {
    p1 = "\x1B[36m[ DEBUG] ";
  } else if (level_ == INFO) {
    p1 = "\x1B[32m[ INFO ] ";
  } else if (level_ == WARNING) {
    p1 = "\x1B[33m[ WARN ] ";
  } else if (level_ == ERROR) {
    p1 = "\x1B[91m[ ERROR] ";
  } else if (level_ == FATAL) {
    p1 = "\x1B[35m[ FATAL] ";
  }

  std::cout << p1 << getNowTime() << " ("<< file_name_ << ":" << line_ << "): \x1B[39m" << str()
            << std::endl;
}
}  // namespace quiche