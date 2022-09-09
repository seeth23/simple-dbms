#ifndef __DBMS_MANAGEMENT__
#define __DBMS_MANAGEMENT__

#include "Database.hpp"
#include "FileSys.hpp"
#include "Parser.hpp"
#include "LexicalAnalyzer.hpp"

#include "Error.hpp"

#include <cstddef>
#include <vector>
#include <cstdio>
#include <utility>
#include <vector>
#include <limits>
#include <map>

class DBMS {
	bool m_database_running;
	Database *m_current_database;
	std::vector<Database*> m_databases;

	FileSys *m_file_sys;
	Parser *m_parser;
	LexicalAnalyzer *m_analyzer;

	size_t m_id;
	
	void input_handle();
	int find_database_index(std::string db) const;
	
	Result show_database(std::string database_name) const;
	Result show_table(std::string table_name) const;

	Result add_record(std::string &table_name, std::vector<std::string> &vals);
	Result use_database(std::string database_name);
	Result delete_table(std::string table_name);
	Result create_table(std::string table_name, const std::map<std::string, ColumnType> &columns);
	Result create_database(std::string database_name);
	Result delete_database(std::string database_name);
	Result delete_record(std::string table_name, size_t id);
public:
	void run();
	
	DBMS();
	~DBMS();
};



#endif
