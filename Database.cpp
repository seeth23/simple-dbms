#include "Database.hpp"
#include "Error.hpp"
#include <ctime>

Database::Database(std::string db_name) 
  : m_name(db_name), m_tables_number(0), m_id(0)
{
}

Database::Database()
	: m_name(""), m_tables_number(0), m_id(0)
{}

const int Database::id() const {
	return this->m_id;
}

std::vector<Table*> Database::get_tables() {
	return this->m_tables;
}

Result Database::add_record(const std::string &table_name, std::vector<std::string> &vals) {
	Table *t = this->get_table(table_name);
	if (!t) return Result(false, table_not_found);
	Result r = t->add_record(this->m_id, vals);
	if (!r.res) return r;
	this->m_id++;
	return Result(true, none);
}

Result Database::load(std::ifstream &ifst) {
	int database_name_len;	
	ifst.read(reinterpret_cast<char*>(&database_name_len), 4);
	char *database_name = (char*)malloc(sizeof(char)*database_name_len+1);
	if (!database_name) {
		std::cerr << "Could not alloc memory for .dbname" << std::endl;
		exit(1);
	}
	ifst.read(database_name, database_name_len);
	database_name[database_name_len] = 0;
	this->m_name = std::string(database_name);
	free(database_name);
	ifst.read(reinterpret_cast<char*>(&this->m_id), 4);
	ifst.read(reinterpret_cast<char*>(&this->m_tables_number), 4);
	for (size_t i = 0; i < this->m_tables_number; i++) {
		this->m_tables.push_back(new Table());
		this->m_tables.back()->load(ifst);
	}
	return Result(true, none);
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

Result Database::show_table(std::string table_name) const {
	if (!table_name.length()) {
		for (auto t : this->m_tables) {
			std::cout
				<< "name: " << t->get_name() 
				<< ";records: " << t->get_records_number()
				<< ";columns: " << t->get_cols_number()
			<< std::endl;
		}
	} else {
		auto t = this->get_table(table_name);
		if (!t) return Result(false, table_not_found);
		t->show_records();
	}
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
