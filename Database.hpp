#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include "Table.hpp"
#include "Types.hpp"
#include "Error.hpp"

#include <string>
#include <vector>
#include <map>

class Database {
	std::vector<Table*> m_tables;
	std::string m_name;
	int m_tables_number;

	int find_table_index(std::string table_name) const;
public:
	Database(std::string db_name);
	~Database();
	Result delete_table(std::string table_name);
	Result create_table(std::map<std::string, ColumnType> cols, std::string name);
	Result show_table(std::string table_name) const;
	Table *get_table(std::string table_name) const;

	const std::string &name() const;
	const int &tables_number() const;
};

#endif
