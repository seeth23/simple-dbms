#include "Database.hpp"
#include "Error.hpp"
#include <ctime>

Database::Database(std::string db_name) 
  : m_name(db_name), m_tables_number(0), m_id(0)
{
}

const int Database::id() const {
	return this->m_id;
}

Result Database::add_record(const std::string &table_name, std::vector<std::string> &vals) {
	Table *t = this->get_table(table_name);
	if (!t) return Result(false, table_not_found);
	Result r = t->add_record(this->m_id, vals);
	if (!r.res) return r;
	this->m_id++;
	return Result(true, none);
}

Result Database::save(std::ofstream &ofst) {
	size_t database_name_len = this->m_name.length();
	
	ofst.write(reinterpret_cast<char*>(&database_name_len), sizeof database_name_len);
	ofst.write(this->m_name.c_str(), database_name_len);
	ofst.write(reinterpret_cast<char*>(&this->m_id), sizeof this->m_id);
	ofst.write(reinterpret_cast<char*>(&this->m_tables_number), sizeof(this->m_tables_number));
	for (auto table: this->m_tables) {
		table->save(ofst);
	}
	ofst.close();
	return Result(true, none);
}

Result Database::load(std::ifstream &ifst) {
	ifst.read(reinterpret_cast<char*>(&this->m_id), sizeof this->m_id);
	ifst.read(reinterpret_cast<char*>(&this->m_tables_number), sizeof this->m_tables_number);
	for (size_t i=0;i<this->m_tables_number;i++) {
// VAR
		size_t table_name_len;
		size_t cols_number;
		char *table_name;
		std::map<std::string, ColumnType> table_cols;
// BEGIN
		ifst.read(reinterpret_cast<char*>(&table_name_len), sizeof table_name_len);
		table_name = (char*)malloc(sizeof(char)*table_name_len);
		if (!table_name) {
			std::cerr << "Could not alloc memory for .table_name" << std::endl;
			exit(1);
		}
		std::cout << "table_name_len " << table_name_len << std::endl;
		ifst.read(table_name, table_name_len);
		table_name[table_name_len] = 0;
		ifst.read(reinterpret_cast<char*>(&cols_number), sizeof cols_number);
		
		for (size_t i = 0; i < cols_number; i++) {
			ColumnType ct;
			size_t col_name_len;
			ifst.read(reinterpret_cast<char*>(&col_name_len), sizeof col_name_len);
			char *col_name = (char*)malloc(sizeof(char)*col_name_len);
			if (!col_name) {
				std::cerr << "Could not alloc memory for .col_name" << std::endl;
				exit(1);
			}
			ifst.read(col_name, col_name_len);
			ifst.read(reinterpret_cast<char*>(&ct), sizeof(int));
			col_name[col_name_len] = 0;
			table_cols.insert(std::pair<std::string, ColumnType>(std::string(col_name), ct));
			free(col_name);
		}
		this->m_tables.push_back(new Table(table_cols, std::string(table_name)));
		free(table_name);
		this->m_tables[i]->load(ifst);
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
