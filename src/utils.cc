#include <string>
#include <fstream>
#include <sys/types.h> 
#include <unistd.h>

#include "utils.hpp"

namespace gazer {

void create_pid_file(const std::string& fpath) {
  std::ofstream fs;

  fs.open(fpath);
  pid_t pid = getpid();

  fs << pid;
  fs.close();
}

void goto_daemon() {
}


}   // namespace gazer
