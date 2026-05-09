#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <string>

class Logger {
  std::string log_str_;

public:
  explicit Logger(const std::string & name)
  : log_str_("[" + name + "]: ")
  {
  }

  void log(const std::string & str)
  {
    std::cout << log_str_ << str << std::endl;
  }
};

#endif
