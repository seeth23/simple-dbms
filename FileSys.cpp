#include "FileSys.hpp"


FileSys::FileSys() {

}

std::ifstream FileSys::open_read(std::string filepath, std::ios::openmode mode) {
  std::ifstream instream(filepath, std::ios::binary | mode);
  if (!instream.is_open()) {
    std::cerr << "Could not open file: " << filepath << " for reading" << std::endl;
    exit(EXIT_FAILURE); 
  }
  return instream;
}

std::ofstream FileSys::open_write(std::string filepath, std::ios::openmode mode) {
  std::ofstream ofstream(filepath, std::ios::binary | mode);
  if (!ofstream.is_open()) {
    std::cerr << "Could not open file for writing: " << filepath << " for writing" << std::endl;
    exit(EXIT_FAILURE); 
  }
  return ofstream; 
}