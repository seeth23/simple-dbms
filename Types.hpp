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

	Result load(std::ifstream &ifst) {
		char *text_field = NULL;
		char *date_field = NULL;
		int number_field;
		double money_field;
		size_t text_field_len;
		switch(type) {
		case text:
			ifst.read(reinterpret_cast<char*>(&text_field_len), sizeof text_field_len);
			text_field = (char*)malloc(sizeof(char)*text_field_len);
			if (!text_field) {
				std::cerr << "Could not alloc memory for .text_field" << std::endl;
				exit(1);
			}
			ifst.read(text_field, text_field_len);
			data.str = text_field;
			break;
		case money:
			ifst.read(reinterpret_cast<char*>(&data.money), sizeof(data.money));
			break;
		case number:
			ifst.read(reinterpret_cast<char*>(&data.num), sizeof(data.num));
			break;
		case date:
			ifst.read(reinterpret_cast<char*>(&text_field_len), sizeof text_field_len);
			text_field = (char*)malloc(sizeof(char)*text_field_len);
			if (!text_field) {
				std::cerr << "Could not alloc memory for .text_field" << std::endl;
				exit(1);
			}
			ifst.read(text_field, text_field_len);
			data.date = Date(std::string(text_field));
			free(text_field);
			break;
		case undefined_type: return Result(false, undefined_typ);
		default:
			std::cerr << "wtf" << std::endl;
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

	Result save(std::ofstream &ofst) {
		ofst.write(reinterpret_cast<char*>(&type), sizeof type);
		switch (type) {
		case text: {
			size_t strl = strlen(data.str)-1;
			ofst.write(reinterpret_cast<char*>(&strl), sizeof strl);
			ofst.write(data.str, strl);
			break;
		 }
		case number:
			ofst.write(reinterpret_cast<char*>(&data.num), sizeof data.num);
			break;
		case money:
			ofst.write(reinterpret_cast<char*>(&data.money), sizeof data.money);
			break;
		case date: {
			char *r = (char*)malloc(data.date.Display().length());
			strcpy(r, data.date.Display().c_str());
			size_t strl = strlen(r);
			ofst.write(reinterpret_cast<char*>(&strl), sizeof strl);
			ofst.write(r, strl);
			free(r);
			break;
		 }
		case undefined_type: throw undefined_typ;
		}
		return Result(true, none);
	}
};

struct Record {
	int id;
	Field **fields;
	int fields_number;

	Record(int flds_num): id(0), fields(nullptr), fields_number(flds_num) {
	}

	Result save(std::ofstream &ofst) {
		ofst.write(reinterpret_cast<char*>(&id), sizeof id);
		for (size_t i = 0;i < fields_number; i++) {
			fields[i]->save(ofst);
		}
		return Result(true, none);
	}

	Result load(std::ifstream &ifst) {
		fields = new Field*[this->fields_number];
		ifst.read(reinterpret_cast<char*>(&id), sizeof id);
		for (size_t i = 0; i < this->fields_number; i++) {
			ColumnType ct;
			ifst.read(reinterpret_cast<char*>(&ct), sizeof(int));
			fields[i] = new Field(ct);
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
