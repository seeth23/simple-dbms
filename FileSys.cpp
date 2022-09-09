#include "FileSys.hpp"

#include <string.h>

std::ifstream open_read(const std::string &filepath, std::ios::openmode mode) {
  std::ifstream instream(filepath, std::ios::binary | mode);
  if (!instream.is_open()) {
    std::cerr << "Could not open file: " << filepath << " for reading" << std::endl;
    exit(EXIT_FAILURE); 
  }
  return instream;
}

std::ofstream open_write(const std::string &filepath, std::ios::openmode mode) {
  std::ofstream ofstream(filepath, std::ios::binary | mode);
  if (!ofstream.is_open()) {
    std::cerr << "Could not open file for writing: " << filepath << " for writing" << std::endl;
    exit(EXIT_FAILURE); 
  }
  return ofstream; 
}

bool close_file(const std::fstream &f) {
	return true;
}

std::vector<std::string> read_directory(const std::string &path) {
	//std::string local_path("saves"); // remove after debug
	std::vector<std::string> dbs_path;
	struct dirent *obj;
	DIR *dir;
	if ((dir = opendir(path.c_str())) != NULL) {
		while ((obj = readdir(dir)) != NULL) {
			if (!strcmp(obj->d_name, ".") || !strcmp(obj->d_name, "..")) continue;
			if (strstr(obj->d_name, ".ldb")) dbs_path.push_back(path+obj->d_name);
		}
	} else {
		std::cerr << "Could not open directory: " << strerror(errno) << std::endl;
		exit(1);
	}
	closedir(dir);
	return dbs_path;
}
