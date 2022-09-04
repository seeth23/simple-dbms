#include "Table.hpp"
#include "Types.hpp"
#include <cstring>
#include <stdexcept>
#include <string>

static size_t id; // id is super-global variable for id in records, its related to all tables and probably databases.

Table::Table(std::map<std::string, ColumnType> cols, std::string table_name)
{
  for (const auto& [key, value] : cols) {
    if (value == undefined_type)
      throw std::invalid_argument("Undefined column type '"+key+"'");
    this->m_columns.push_back(new Column(key, value));
  } 
  this->m_name = table_name;
  this->m_cols_number = cols.size();
  this->m_records_number = 0;
}

Table::~Table() {
  for (auto i : this->m_columns) {
    delete i;
  }
  for (auto i: this->m_records) {
    delete i;
  }
}

bool Table::add_record(std::vector<std::string> values) {
  Field **fields = new Field*[this->m_columns.size()];
  RecordData rd;
  std::cout << "values[0]: " << values[0] << std::endl;
  for (size_t i = 0; i < this->m_columns.size(); i++) {
    ColumnType ct = this->m_columns[i]->column_type;
    switch (ct) {
    case text:
      rd.str = (char*)malloc(sizeof(char)*values[i].length());
      if (!rd.str) {
        std::cerr << "Failed to alloc mem for rd.str" << std::endl;
        exit(1);
      }
      strcpy(rd.str, values[i].c_str());
      fields[i] = new Field(ct, rd);
      break;
    case number:
      rd.num = std::stoi(values[i]);
      fields[i] = new Field(ct, rd);
      break;
    case money:
      rd.money = std::stod(values[i]);
      fields[i] = new Field(ct, rd);
      break;
    case date:
      rd.date = Date(1, 1, 1996);
      fields[i] = new Field(ct, rd);
      break;
    case undefined_type:
      throw std::invalid_argument("Error: undefined type.");
      break;
    }
  }
  this->m_records.push_back(new Record(id, fields, this->m_columns.size()));

  for (size_t i = 0; i < this->m_columns.size(); i++) {
    delete fields[i];
  }
  delete [] fields;
  
  m_records_number++;
  id++;
  return true;
}

bool Table::delete_record(int id) {
  int i = this->find_record_index(id);
  if (i == -1) {
    std::cerr << "Record was not found" << std::endl;
    return false;
  }
  delete this->m_records[i];
  this->m_records.erase(this->m_records.begin()+i);
  this->m_records_number--;
  return true;
}

int Table::find_record_index(int id) const {
  for (size_t i = 0; i < this->m_records_number; i++) { 
    if (this->m_records[i]->id == id) return i;
  }
  return -1;
}

// TODO: think of how to format output. Should be the same for all the types, not chaotic.
void Table::show_records() const {
  std::cout << "Records: " << this->m_records_number << std::endl;
  std::cout << "Columns: " << this->m_cols_number << std::endl;
  for (size_t i = 0; i < this->m_records_number; i++) {
    std::cout << this->m_records[i]->id << std::endl;
    
    const auto t = this->m_records[i]->fields;
    for (size_t j = 0; j < this->m_cols_number; j++) {
      printf("id %d ", this->m_records[i]->id);
      switch (t[j]->type) {
      case text:
        printf("value %s\n", t[j]->data.str);
        break;
      case number:
        printf("value %d\n", t[j]->data.num);
        break;
      case money:
        printf("value %f\n", t[j]->data.money);
        break;
      case date:
        printf("value %s\n", t[j]->data.date.Display().c_str());
        break;
      case undefined_type:
        break;
      }
    }
  }
}

const std::string &Table::get_name() const {
  return this->m_name;
}

int Table::get_cols_number() const {
  return this->m_cols_number;
}

int Table::get_records_number() const {
  return this->m_records_number;
}

