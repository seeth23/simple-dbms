#ifndef __FILE_SYS__
#define __FILE_SYS__

#include <fstream>
#include <iostream> 
#include <vector>
#include <dirent.h>

std::ifstream open_read(const std::string &filepath, std::ios::openmode mode);
std::ofstream open_write(const std::string &filepath, std::ios::openmode mode);
bool close_file(std::fstream f);
// change return type to std::vector<std::string> or just pointer to heap array
std::vector<std::string> read_directory(const std::string &path); 


#endif
