#include "DatabaseManagement.hpp"
#include "Database.hpp"
#include "LexicalAnalyzer.hpp"

#include <stdexcept>

DBMS::DBMS() {
  this->m_file_sys = new FileSys();
  this->m_parser = new Parser();
  this->m_analyzer = new LexicalAnalyzer();
  
  this->m_database_running = false;
  this->m_current_database = nullptr;
}

DBMS::~DBMS() {
  delete this->m_file_sys;
  delete this->m_parser;
  delete this->m_analyzer;
}

bool DBMS::create_database(std::string database_name) {
  this->m_databases.push_back(new Database(database_name));
  return true;
}

bool DBMS::delete_database(std::string database_name) {
  int i = this->find_database_index(database_name);
  if (i == -1) {
    std::cerr << "Database was not found" << std::endl;
    return false;
  }
  delete this->m_databases[i];
  this->m_databases.erase(this->m_databases.begin()+i);
  return true;
}

bool DBMS::open_database(std::string database_name) {
  int i = this->find_database_index(database_name);
  if (i == -1) { 
    std::cerr << "Database was not found" << std::endl;
    return false;
  }
  this->m_current_database = this->m_databases[i];
  return true;
}
// TODO: think of how to format output. Should be the same for all the types, not chaotic.
void DBMS::show_database(std::string database_name) {
}

int DBMS::find_database_index(std::string database_name) {
  for (size_t i = 0; i < this->m_databases.size(); i++) {
    if (this->m_databases[i]->get_name() == database_name) return i;
  }
  return -1;
}
// TODO: Create const error logs for logging every error, and invoke it with function and enum value. 
// TODO: Move statements inside case: {} to another private methods so 'void DBMS::input_hande() {...}' will be more clean and nice.

void DBMS::input_handle() {
  std::string cmd = "";
  std::vector<std::string> commands;

  while (std::cin >> cmd) {
    commands.push_back(cmd);
    if (cmd == "END") break;    
  }

  if (commands.size()) {
    if (commands.back() == "END") {
      auto r = this->m_analyzer->parse_expression(commands);
      std::string name = this->m_analyzer->get_name();
      auto values = this->m_analyzer->get_values();   
      switch (r) {
        case use: {
          int i = this->find_database_index(name);
          if (i == -1) {
            std::cerr << "Could not find database '"+name+"'" << std::endl;
          } else {
            this->m_current_database = this->m_databases[i];
            std::cout << "Using database '"+name+"'" << std::endl;
          }
          break;
        }
        case add: {
          Table *t = this->m_current_database->get_table(name);
          if (!t) {
            std::cerr << "Table not found: '"+name+"'" << std::endl;
          } else {
            
            bool r = t->add_record(values);
            if (!r) {
              std::cerr << "Could not add record" << std::endl;
            }
          }
          break;
        }
        case deltbl: {
          if (this->m_current_database) {
            bool r = this->m_current_database->delete_table(name);
            if (!r) {
              std::cerr << "Could not delete table '"+name+"'" << std::endl;
            } else {
              std::cout << "Successfully deleted table '"+name+"'" << std::endl;
            }
          } else {
            std::cerr << "Current database is not chosen" << std::endl;
          }
          break;
        }
        case deldb: {
          if (this->m_current_database->get_name() == name) {
            std::cerr << "Can't delete database: currently used" << std::endl;
          }
          bool r = this->delete_database(this->m_analyzer->get_name());
          if (r) {
            std::cout << "Successfully deleted database '"+name+"'" << std::endl;
            
          } else
            std::cerr << "Could not delete database '"+name+"'" << std::endl;
          break;
        }
        case showtbl:
          this->m_current_database->show_table(name);
          break;
        case showdb:
          this->show_database(name);
          break;
        case crttbl:
          if (this->m_current_database) {
            auto r = this->m_current_database->create_table(this->m_analyzer->get_columns(), name);
            if (this->m_analyzer->get_columns().size() == 0) {
              std::cerr << "No cols" << std::endl;
              exit(0);
            }
            if (!r) {
              std::cerr << "Could not create table '"+name+"'" << std::endl;
            } else {
              std::cout << "Successfully created table '"+name+"'"+" in database '"+this->m_current_database->get_name() << std::endl;
            }
          }
          else
            std::cerr << "Database is not chosen" << std::endl;
          break;
        case crtdb:
          if (this->create_database(this->m_analyzer->get_name())) {
            std::cout << "Successfully created database '"+name+"'" << std::endl;
          } else {
            std::cout << "Could not create database '"+name+"'" << std::endl;
          }
          break;
        case undefined_operation:
          std::cerr << "Error: undefined type." << std::endl;
          break;
      }
    } else
      throw std::invalid_argument("Wrong syntax");
  }

  if (std::cin.fail()) {
    std::cout << "EOF reached." << std::endl;
    clearerr(stdin);
    std::cin.clear();
    this->m_database_running = false;
  }
}

void DBMS::run() {
  m_database_running = true;
  while (m_database_running) {
    input_handle();
  }
}
