#ifndef __TABLE_HPP__
#define __TABLE_HPP__

#include "Types.hpp"
#include "Error.hpp"

#include <vector>
#include <map>
#include <fstream>

class Table {
	std::vector<Record*> m_records;
	std::vector<Column*> m_columns;
	size_t m_cols_number;
	size_t m_records_number;

	std::string m_name;
public:
	Table(std::map<std::string, ColumnType> cols, std::string table_name);  
	~Table();
	Result add_record(size_t id, std::vector<std::string> values);
	Result delete_record(size_t id);
	void show_records() const;
	int find_record_index(int id) const;

	Result save(std::ofstream &ofst);
	Result load(std::ifstream &ifst);

	const std::string &get_name() const;
	int get_cols_number() const;
	int get_records_number() const;
};


#endif
