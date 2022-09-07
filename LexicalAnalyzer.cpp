#include "LexicalAnalyzer.hpp"
#include "Error.hpp"
#include "Types.hpp"

LexicalAnalyzer::LexicalAnalyzer(): m_id(0) {
}

const std::string &LexicalAnalyzer::name() const {
	return this->m_name;
}

const std::vector<std::string> &LexicalAnalyzer::values() const {
	return this->m_values;
}

const std::map<std::string, ColumnType> &LexicalAnalyzer::columns() const {
	return this->m_table_signature;
}

size_t LexicalAnalyzer::id() const {
	return this->m_id;
}

static bool IsKeyword(const std::string &key) {
	for (size_t i = 0; i < keywords_size; i++)
		if (KEYWORDS[i] == key) return true;
	return false;
}

static ColumnType column_type(std::string st) {
	if (st == "TEXT") return text;
	if (st == "NUMBER") return number;
	if (st == "MONEY") return money;
	if (st == "DATE") return date;
	return undefined_type;
}

static Operations command_type(std::string cmd)  {
	if      (cmd == "USE") return use;
	else if (cmd == "ADD") return add;
	else if (cmd == "DELTBL") return deltbl;
	else if (cmd == "DELDB") return deldb;
	else if (cmd == "SHOWTBL") return showtbl;
	else if (cmd == "SHOWDB") return showdb;
	else if (cmd == "CRTTBL") return crttbl;
	else if (cmd == "CRTDB") return crtdb;
	else if (cmd == "DELREC") return delrec;
	return undefined_operation;
}

Operations LexicalAnalyzer::parse_expression(std::vector<std::string> tokens) {
	// TODO: Check if some token is KEYWORD
	if (tokens[1].size() == 0) throw zero_length_name;
	this->m_name = tokens[1];
	Operations operation;
	switch ((operation = command_type(tokens[0]))) {
		case add:
			this->m_values.clear();
			for (size_t i = 2; i < tokens.size()-1; i++) {
				auto n = ILLEGAL_SYMBOLS.find(tokens[i]);
				if (n != ILLEGAL_SYMBOLS.end()) throw invalid_token; //throw std::invalid_argument("Invalid token in expression");
				this->m_values.push_back(tokens[i]);
			}
			break;
		case delrec:
			if (IsKeyword(this->m_name)) throw keyword;
			this->m_id = stoi(tokens[2]);
			return operation;
		/* all next N cases are the same: only table's or db's names matter */
		case use:
		case deldb:
		case deltbl:
		case showtbl:
		case showdb:
		case crtdb:
			if (IsKeyword(this->m_name)) throw keyword;
			return operation;
		case crttbl: /* parse key:value to create table. */
			this->m_name = tokens[1];
			this->m_table_signature.clear();
			for (size_t i = 2; i < tokens.size()-1; i++) {
				int delimeter_pos = tokens[i].find(':');
				std::string key = tokens[i].substr(0, delimeter_pos);
				if (IsKeyword(key)) throw keyword;
				std::string value = tokens[i].substr(delimeter_pos+1, tokens[i].length()-1);
				ColumnType col_type = column_type(value);
				if (col_type == undefined_type) {
					throw undefined_typ;
				}
				this->m_table_signature.insert(std::pair<std::string, ColumnType>(key, col_type));
			}
			break;
		case undefined_operation:
			throw undefined_op;
	}
	return operation;
}
