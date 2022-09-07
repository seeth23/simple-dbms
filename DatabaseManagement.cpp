#include "DatabaseManagement.hpp"
#include "Database.hpp"
#include "Error.hpp"
#include "LexicalAnalyzer.hpp"

#include <stdexcept>

DBMS::DBMS() {
	this->m_file_sys = new FileSys();
	this->m_parser = new Parser();
	this->m_analyzer = new LexicalAnalyzer(); 
	this->m_database_running = false;
	this->m_current_database = nullptr;
	this->m_id = 0;
}

DBMS::~DBMS() {
	delete this->m_file_sys;
	delete this->m_parser;
	delete this->m_analyzer;
}

Result DBMS::create_database(std::string database_name) {
	if (this->find_database_index(database_name) != -1) return Result(false, database_exists);
	this->m_databases.push_back(new Database(database_name));
	return Result(true, none);
}

Result DBMS::delete_database(std::string database_name) {
	int i = this->find_database_index(database_name);
	if (i == -1) {
		return Result(false, database_not_found);
	}
	delete this->m_databases[i];
	this->m_databases.erase(this->m_databases.begin()+i);
	return Result(true, none);
}

Result DBMS::use_database(std::string database_name) {
	int i = this->find_database_index(database_name);
	if (i == -1) return Result(false, database_not_found);
	else this->m_current_database = this->m_databases[i];
	return Result(true, none);
}

Result DBMS::add_record(std::string &table_name, std::vector<std::string> &vals) {
	Table *t = this->m_current_database->get_table(table_name);
	if (!t) return Result(false, table_not_found);
	bool r = t->add_record(this->m_id, vals);
	if (!r) return Result(false, could_not_add_record);
	this->m_id++;
	return Result(true, none);
}

Result DBMS::create_table(std::string table_name, const std::map<std::string, ColumnType> &columns) {
	if (this->m_current_database) {
		if (this->m_analyzer->columns().size() == 0) return Result(false, zero_length_name);
		if (this->m_current_database->get_table(table_name)) return Result(false, table_exists); // if not nullptr then table already exists
		Result r = this->m_current_database->create_table(this->m_analyzer->columns(), table_name);
		if (!r.res) return r;
	} else return Result(false, database_not_chosen);
	return Result(true, none);
}

Result DBMS::delete_table(std::string table_name) {
	if (this->m_current_database) {
		Result r = this->m_current_database->delete_table(table_name);
		if (!r.res) return r;
	} else return Result(false, database_not_chosen);
	return Result(true, none);
}

Result DBMS::show_table(std::string table_name) {
	if (this->m_current_database) {
		Result r = this->m_current_database->show_table(table_name);
		if (!r.res) return r;
	}
	else return Result(false, database_not_chosen);
	return Result(true, none);
}

Result DBMS::delete_record(std::string table_name, size_t id) {
	if (this->m_current_database) {
		Table *t = this->m_current_database->get_table(table_name);
		if (!t) return Result(false, table_not_found);
		Result r = t->delete_record(id);
		if (!r.res) return r;
	} else return Result(false, database_not_chosen);
	return Result(true, none);
}

// TODO: think of how to format output. Should be the same for all the types, not chaotic.
Result DBMS::show_database(std::string database_name) {
	std::cout << "Databases" << std::endl;
	std::cout << "---------------------" << std::endl;
	for (auto db : this->m_databases) {
		std::cout << "Name: " << db->name() << ", tables: " << (*db).tables_number() << std::endl;
	}
	return Result(true, none);
}

int DBMS::find_database_index(std::string database_name) {
	for (size_t i = 0; i < this->m_databases.size(); i++) {
		if (this->m_databases[i]->name() == database_name) return i;
	}
	return -1;
}

void DBMS::input_handle() {
	std::string cmd = "";
	std::vector<std::string> commands;

	while (std::cin >> cmd) {
		commands.push_back(cmd);
		if (cmd == "END") break;    
	}

	if (commands.size()) {
		if (commands.back() == "END") {
			Operations r = this->m_analyzer->parse_expression(commands);
			std::string name = this->m_analyzer->name();
			std::vector<std::string> values = this->m_analyzer->values(); 
			Result res;
			switch (r) {
			case use:
				res = this->use_database(name);
				if (!res.res) throw res.error;
				std::cout << "using database '"+name+"'" << std::endl;
				break;
			case add:
				res = this->add_record(name, values);
				if (!res.res) throw res.error;
				std::cout << "added record to table '"+name+"'" << std::endl;
				break;
			case deltbl:
				res = this->delete_table(name);
				if (!res.res) throw res.error;
				std::cout << "deleted table '"+name+"'" << std::endl;
				break;
			case deldb:
				res = this->delete_database(name);
				if (!res.res) throw res.error;
				std::cout << "deleted database '"+name+"'" << std::endl;
				break;
			case delrec:
				res = this->delete_record(name, this->m_analyzer->id());
				if (!res.res) throw res.error;
				std::cout << "deleted record '" << this->m_analyzer->id() << "'" << std::endl;
				break;
			case showtbl:
				res = this->show_table(name); // TODO
				if (!res.res) throw res.error;
				break;
			case showdb:
				res = this->show_database(name); // TODO
				if (!res.res) throw res.error;
				break;
			case crttbl:
				res = this->create_table(name, this->m_analyzer->columns());
				if (!res.res) throw res.error;
				std::cout << "created table '"+name+"'" << std::endl;
				break;
			case crtdb:
				res = this->create_database(name);
				if (!res.res) throw res.error;
				std::cout << "created database '"+name+"'" << std::endl;
				break;
			case undefined_operation: throw undefined_typ;
				break;
			}
		} else throw wrong_syntax;
	}

	if (std::cin.fail()) {
		std::cout << "EOF reached." << std::endl;
		clearerr(stdin);
		std::cin.clear();
		this->m_database_running = false;
	}
}

static std::string error_message(Errors e, std::string arg = "") {
	switch (e) {
	case undefined_op: return "Undefined operation"+arg;
	case undefined_typ: return "Undefined type"+arg;
	case invalid_token: return "Invalid token"+arg;
	case wrong_syntax: return "Wrong syntax"+arg;
	case keyword: return "Use of keyword"+arg;
	case database_not_chosen: return "Database not chosen"+arg;
	case database_not_found: return "Database not found"+arg;
	case table_not_found: return "Table not found"+arg;
	case record_not_found: return "Record not found"+arg;
	case could_not_add_record: return "Could not add record"+arg;
	case could_not_delete_table: return "Could not delete table"+arg;
	case could_not_delete_database: return "Could not delete database"+arg;
	case could_not_create_table: return "Could not create table"+arg;
	case could_not_create_database: return "Could not create database"+arg;
	case no_columns: return "No columns were provided"+arg;
	case zero_length_name: return "Name cannot be zero"+arg;
	case table_exists: return "Table already exists"+arg;
	case database_exists: return "Database already exists"+arg;
	case none:
		break;
	}
	return "";
}

void DBMS::run() {
start:
	m_database_running = true;
	try {
		while (m_database_running)
			input_handle();
	} catch (Errors err) {
		switch (err) {
		case undefined_op:
		case undefined_typ:
		case invalid_token:
		case wrong_syntax:
		case database_not_chosen:
		case database_not_found:
		case table_not_found:
		case record_not_found:
		case could_not_add_record:
		case could_not_delete_table:
		case could_not_delete_database:
		case could_not_create_table:
		case could_not_create_database:
		case no_columns:
		case zero_length_name:
		case keyword:
		case table_exists:
		case database_exists:
			std::cout << "Error: " << error_message(err) << std::endl;
			goto start;
		case none:
			goto start;
			break;
		}
	}
}
