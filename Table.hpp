#ifndef __TABLE_HPP__
#define __TABLE_HPP__

#include "Types.hpp"

#include <vector>
#include <map>



class Table {
  std::vector<Record*> m_records;
  std::vector<Column*> m_columns;
  size_t m_cols_number;
  size_t m_records_number;
  //static size_t id;
  std::string m_name;
public:
  Table(std::map<std::string, ColumnType> cols, std::string table_name);  
  ~Table();
  bool add_record(std::vector<std::string> values);
  bool delete_record(int id);
  void show_records() const;
  int find_record_index(int id) const;

  const std::string &get_name() const;
  int get_cols_number() const;
  int get_records_number() const;
};


#endif