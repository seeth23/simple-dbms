#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <cstddef>
#include <stdexcept>
#include <string>
#include <iostream>

#include <string.h>
#include <fstream>

#include "Error.hpp"

struct Date {
	size_t day;
	size_t month;
	size_t year; 
	Date(): day(0), month(0), year(0) {}
	Date(size_t d, size_t m, size_t y) : day(d), month(m), year(y) {}
	Date(const Date &d) { day = d.day; month = d.month; year = d.year; }
	std::string Display() const {
		return std::to_string(day)+"/"+std::to_string(month)+"/"+std::to_string(year);
	}
	Date(std::string s) {} // TODO use date parser here as well
};

enum Operations {
	use,
	add,
	deltbl,
	deldb,
	delrec,
	showtbl,
	showdb,
	crttbl,
	crtdb,
	undefined_operation,
};

enum ColumnType {
	text, // text
	number, // int
	money, // double
	date, // Date
	undefined_type, // unknown type
};

struct Column {
	std::string column_name;
	ColumnType column_type;

	Column(std::string col, ColumnType typ) {
		this->column_name = col;
		this->column_type = typ;
	}
};

union RecordData {
	char *str;
	int num;
	double money;
	Date date;
	RecordData(): num(0) {}  
};

struct Field {
	ColumnType type;
	RecordData data;

	Field(ColumnType ct): type(ct) {}
	Field() : type(undefined_type) {}


	Result load(std::ifstream &ifst) {
		ifst.read(reinterpret_cast<char*>(&type), 4);
		switch (type) {
		case text: {
			int text_field_len;
			ifst.read(reinterpret_cast<char*>(&text_field_len), 4);
			char *text_field = (char*)malloc(sizeof(char)*text_field_len+1);
			if (!text_field_len) {
				std::cerr << "Error: tried to call malloc .txtfld for " << text_field_len << " bytes" << std::endl;
				exit(1);
			}
			ifst.read(text_field, text_field_len);
			text_field[text_field_len] = 0;
			data.str = text_field;
			break;
		 }
		case money:
			ifst.read(reinterpret_cast<char*>(&data.money), sizeof data.money);
			break;
		case number:
			ifst.read(reinterpret_cast<char*>(&data.num), 4);
			break;
		case date: {
			int text_field_len;
			ifst.read(reinterpret_cast<char*>(&text_field_len), 4);
			char *text_field = (char*)malloc(sizeof(char)*text_field_len+1);
			if (!text_field_len) {
				std::cerr << "Error: tried to call malloc .txtfld for " << text_field_len << " bytes" << std::endl;
				exit(1);
			}
			ifst.read(text_field, text_field_len);
			text_field[text_field_len] = 0;
			data.date = Date(std::string(text_field));
			free(text_field);
			break;
	 }
		case undefined_type: 
			std::cerr << "Undefined type was read" << std::endl;
			exit(1);
		}


		return Result(true, none);
	}
	
	Field(Field &f) {
		this->type = f.type;
		switch (f.type) {
		case text:
			this->data.str = (char*)malloc(sizeof(char)*strlen(f.data.str));
			if (!this->data.str) {
				std::cerr << "Failed to alloc mem for .str" << std::endl;
				exit(1);
			}
			strcpy(this->data.str, f.data.str);
			break;
		case number:
			this->data.num = f.data.num;
			break;
		case money:
			this->data.money = f.data.money;
			break;
		case date:
			this->data.date = f.data.date;
			break;
		case undefined_type:
			break;
		}
	}

	Field(ColumnType typ, const RecordData &rd)
		: type(typ) {
		switch (typ) {
		case text:
			this->data.str = (char*)malloc(sizeof(char)*strlen(rd.str));
			if (!this->data.str) {
				std::cerr << "Failed to alloc mem for .str" << std::endl;
				exit(1);
			}
			strcpy(this->data.str, rd.str);
		case number:
			this->data.num = rd.num;
		case money:
			this->data.money = rd.money;
		case date:
			this->data.date = rd.date;
		case undefined_type:
			break;
		}
	}

	~Field() {
		switch (this->type) {
		case text:
			if (data.str) {
				free(data.str);
			}
			break;
		case number:
		case money:
		case date:
		case undefined_type:
			break;
		}
	}
};

struct Record {
	int id;
	Field **fields;
	int fields_number;

	Record(int flds_num): id(0), fields(nullptr), fields_number(flds_num) {
		this->fields = new Field*[flds_num];
	}

	Result load(std::ifstream &ifst) {
		ifst.read(reinterpret_cast<char*>(&id), 4);	
		for (size_t i = 0; i < fields_number; i++) {
			fields[i] = new Field();
			fields[i]->load(ifst);
		}
		return Result(true, none);
	}

	Record(int id, Field **f, int f_number): id(id), fields_number(f_number) {
		this->fields = new Field*[f_number];
		for (int i = 0; i < f_number; i++) {
			this->fields[i] = new Field(*f[i]);
		}
	}

	~Record() {
		for (int i = 0; i < fields_number; i++) {
			delete fields[i];
		}
		delete []fields;
	}
};

#endif
