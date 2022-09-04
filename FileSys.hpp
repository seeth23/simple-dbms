#ifndef __FILE_SYS__
#define __FILE_SYS__

#include <fstream>
#include <iostream> 
#include <fstream>

class FileSys {
public:
  FileSys();
  std::ifstream open_read(std::string filepath, std::ios::openmode mode);
  std::ofstream open_write(std::string filepath, std::ios::openmode mode);

};

#endif