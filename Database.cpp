#include "Database.hpp"

Database::Database(std::string db_name) 
  : m_name(db_name)
{
}

Database::~Database() {
  for (auto table : this->m_tables) {
    delete table;
  }
}

int Database::find_table_index(std::string table_name) const {
  for (size_t i = 0; i < this->m_tables.size(); i++) {
    if (this->m_tables[i]->get_name() == table_name) return i;
  }
  return -1;
}

bool Database::create_table(std::map<std::string, ColumnType> cols, std::string name) {
  this->m_tables.push_back(new Table(cols, name));
  return true;
}

// TODO: think of how to format output. Should be the same for all the types, not chaotic.
void Database::show_table(std::string table_name) const {
  for (auto t : this->m_tables) {
    std::cout << t->get_name() << ": " << std::endl;
    t->show_records();
  }
}

bool Database::delete_table(std::string table_name) {
  int i = this->find_table_index(table_name);
  if (i == -1) {
    std::cerr << "Table was not found" << std::endl;
    return false;
  }
  delete this->m_tables[i];
  this->m_tables.erase(this->m_tables.begin()+i);
  return true;
}

Table *Database::get_table(std::string table_name) const {
  for (auto table: this->m_tables) {
    if (table->get_name() == table_name) { return table; }
  }
  return nullptr;
}

const std::string &Database::get_name() const {
  return this->m_name;
}