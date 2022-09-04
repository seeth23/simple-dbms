#ifndef __DBMS_MANAGEMENT__
#define __DBMS_MANAGEMENT__

#include "Database.hpp"
#include "FileSys.hpp"
#include "Parser.hpp"
#include "LexicalAnalyzer.hpp"

#include <cstddef>
#include <vector>
#include <cstdio>
#include <utility>
#include <vector>
#include <limits>

class DBMS {
  bool m_database_running;
  Database *m_current_database;
  std::vector<Database*> m_databases;

  FileSys *m_file_sys;
  Parser *m_parser;
  LexicalAnalyzer *m_analyzer;

  void input_handle();
  int find_database_index(std::string db); // TODO: find database by name and attach it to *m_current_database;
  bool open_database(std::string database_name);
  bool delete_database(std::string database_name);
  bool create_database(std::string database_name);
  void show_database(std::string database_name);
public:
  void run();
  
  DBMS();
  ~DBMS();
};



#endif