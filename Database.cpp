#include "Database.hpp"
#include "Error.hpp"


Database::Database(std::string db_name) 
  : m_name(db_name), m_tables_number(0)
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

Result Database::create_table(std::map<std::string, ColumnType> cols, std::string name) {
  this->m_tables.push_back(new Table(cols, name));
  this->m_tables_number++;
  return Result(true, none);
}

// TODO: think of how to format output. Should be the same for all the types, not chaotic.
Result Database::show_table(std::string table_name) const {
  auto t = this->get_table(table_name);
  if (!t) return Result(false, table_not_found);
  t->show_records();
  return Result(true, none);
}

Result Database::delete_table(std::string table_name) {
  int i = this->find_table_index(table_name);
  if (i == -1) {
    return Result(false, table_not_found);
  }
  delete this->m_tables[i];
  this->m_tables.erase(this->m_tables.begin()+i);
  this->m_tables_number--;
  return Result(true, none);
}

Table *Database::get_table(std::string table_name) const {
  for (auto table: this->m_tables) {
    if (table->get_name() == table_name) { return table; }
  }
  return nullptr;
}

const int &Database::tables_number() const {
  return this->m_tables_number;
}

const std::string &Database::name() const {
  return this->m_name;
}
