#ifndef __GAZER_UTILS_HPP__
#define __GAZER_UTILS_HPP__

#include <string>

namespace gazer {

void create_pid_file(const std::string& fpath);
void demonize();

}   // namespace gazer

#endif   // __GAZER_UTILS_HPP__
