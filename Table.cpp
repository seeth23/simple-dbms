#include "Table.hpp"
#include "Types.hpp"
#include <cstring>
#include <stdexcept>
#include <string>

Table::Table(std::map<std::string, ColumnType> cols, std::string table_name)
{
	for (const auto& [key, value] : cols) {
		if (value == undefined_type) throw std::invalid_argument("Undefined column type '"+key+"'");
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

static Result parse_date(std::string date, int *d, int *m, int *y) {
	int day = (date[0]-48)*10+date[1]-48;
	int month = (date[3]-48)*10+date[4]-48;
	int year = (((date[6]-48)*10+(date[7]-48))*10+(date[8]-48))*10+(date[9]-48);

	if (date[2] != '/' || date[5] != '/')
		return Result(false, wrong_date_format);
	if ((day < 1 || day > 31) || (month < 1 || month > 12) || (year < 0 || year > INT32_MAX))
		return Result(false, wrong_date_format);

	*d = day; *m = month; *y = year;

	return Result(true, none);
}

Result Table::add_record(size_t id, std::vector<std::string> values) {
	Field **fields = new Field*[this->m_columns.size()];
	RecordData rd;
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
			case date: {
				int d, m, y;
				Result r = parse_date(values[i], &d, &m, &y);
				if (!r.res) return r;
				rd.date = Date(d, m, y);
				fields[i] = new Field(ct, rd);
				break;
			 }
			case undefined_type:
				return Result(false, undefined_typ);
		}
	}
	this->m_records.push_back(new Record(id, fields, this->m_columns.size()));

	for (size_t i = 0; i < this->m_columns.size(); i++) {
		delete fields[i];
	}
	delete []fields;

	m_records_number++;
	return Result(true, none);
}

Result Table::delete_record(size_t id) {
	int i = this->find_record_index(id);
	if (i == -1) {
		return Result(false, record_not_found);
	}
	delete this->m_records[i];
	this->m_records.erase(this->m_records.begin()+i);
	this->m_records_number--;
	return Result(true, none);
}

int Table::find_record_index(int id) const {
	for (size_t i = 0; i < this->m_records_number; i++) { 
		if (this->m_records[i]->id == id) return i;
	}
	return -1;
}

static std::string colttos(ColumnType ct) {
	switch (ct) {
		case text: return "TEXT";
		case number: return "NUMBER";
		case date: return "DATE";
		case money: return "MONEY";
		case undefined_type: throw undefined_typ;
	}
	return "";
}

void Table::show_records() const {
	std::cout << "---------------------------------------------------------------------------" << std::endl;
	std::cout << "records: " << this->m_records_number << "; columns: " << this->m_cols_number << std::endl;
	std::cout << "---------------------------------------------------------------------------" << std::endl;
	std::cout << "ID | ";
	for (size_t i = 0; i < this->m_columns.size(); i++) {
		std::cout << this->m_columns[i]->column_name << ":" << colttos(this->m_columns[i]->column_type) << " | ";
	}
	std::cout << std::endl;
	for (size_t i = 0; i < this->m_records_number; i++) {
		const auto t = this->m_records[i]->fields;
		printf("%d\t|\t", this->m_records[i]->id);
		for (size_t j = 0; j < this->m_cols_number; j++) {
			switch (t[j]->type) {
				case text:
					printf("%s\t|\t", t[j]->data.str);
					break;
				case number:
					printf("%d\t|\t", t[j]->data.num);
					break;
				case money:
					printf("%f\t|\t", t[j]->data.money);
					break;
				case date:
					printf("%s\t|\t", t[j]->data.date.Display().c_str());
					break;
				case undefined_type:
					break;
			}
		}
		std::cout << std::endl;
	}
	std::cout << "---------------------------------------------------------------------------" << std::endl;
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
