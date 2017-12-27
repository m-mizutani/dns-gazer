#include <string>
#include <fstream>
#include <sys/types.h> 
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>
#include <fcntl.h>

#include "utils.hpp"


namespace gazer {

void create_pid_file(const std::string& fpath) {
  std::ofstream fs;

  fs.open(fpath);
  pid_t pid = getpid();

  fs << pid;
  fs.close();
}

void demonize() {
  pid_t pid;
  signal(SIGHUP, SIG_IGN);
    
  if ((pid = fork()) == -1) {
    return;
  } else if (pid != 0) {
    exit(0);
  }

  setsid();
  
  /*
  // Disable stdin/stdout/stderr
  int fd = open("/dev/null", O_RDWR, 0);
  if ((fd != -1)) {
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    
    close(fd);
  }
  */  
  return;
}


}   // namespace gazer
