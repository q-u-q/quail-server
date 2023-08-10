
#include "quiche_platform_impl/quiche_logging_impl.h"

namespace quiche {
MyLog::~MyLog(){
  if (level_ < INFO) {
      return;
    }

    std::string p1("\x1B[34mLog(");
    if (level_ == VERBOSE) {
      p1 = "\x1B[34mVLog(";
    }else if(level_ == DEBUG) {
      p1 = "\x1B[36mDLog(";
    }else if(level_ == INFO) {
      p1 = "\x1B[32mILog(";
    }else if(level_ == WARNING) {
      p1 = "\x1B[33mWLog(";
    }else if(level_ == ERROR) {
      p1 = "\x1B[91mELog(";
    }else if(level_ == FATAL) {
      p1 = "\x1B[35mFLog(";
    }

    std::cout << p1 << file_name_ << ":" << line_ << "): \x1B[39m" << str()
              << std::endl;
}
}