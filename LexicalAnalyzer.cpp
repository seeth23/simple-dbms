#include "LexicalAnalyzer.hpp"

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

bool LexicalAnalyzer::is_keyword(const std::string &key) {
	for (size_t i = 0; i < keywords_size; i++)
		if (KEYWORDS[i] == key) return true;
	return false;
}

static bool is_legal_char(const char &c) {
	auto r = ILLEGAL_SYMBOLS.find(c);
	if (r != ILLEGAL_SYMBOLS.end()) return false;
	return true;
}

bool LexicalAnalyzer::is_legal_name(const std::string &name) {
	for (size_t i = 0; i < name.length(); i++) {
		if (!is_legal_char(name[i])) return false;
	}
	return true;
}

ColumnType LexicalAnalyzer::column_type(std::string &st) {
	if (st == "TEXT") return text;
	if (st == "NUMBER") return number;
	if (st == "MONEY") return money;
	if (st == "DATE") return date;
	return undefined_type;
}

// parse cols that are used only when table is created
// COLUMN_NAME:COLUMN_TYPE(no spaces)
void LexicalAnalyzer::parse_columns(std::vector<std::string> tokens) {
	this->m_table_signature.clear();
	for (size_t i = 2;i < tokens.size()-1;i++) {
		int delimeter_pos = tokens[i].find(':');
		std::string key = tokens[i].substr(0, delimeter_pos);
		
		if (is_keyword(key)) 
			throw keyword; // col name cannot be keyword
										 
		std::string value = tokens[i].substr(delimeter_pos+1, tokens[i].length()-1);
		ColumnType col_type = column_type(value);

		if (col_type == undefined_type) 
			throw undefined_typ;

		this->m_table_signature.insert(std::pair<std::string, ColumnType>(key, col_type));
	}
}

// parses values that are used in ADD expression
void LexicalAnalyzer::parse_values(std::vector<std::string> tokens) {
	this->m_values.clear();
	std::string v;
	for (size_t i=2;i<tokens.size()-1;i++) {
		if (!is_legal_name(tokens[i])) throw illegal_name;
		if (tokens[i][0] == '\'') {
			while (i < tokens.size()) {
				v += tokens[i]+' ';
				if (tokens[i][tokens[i].length()-1] == '\'') break;
				i++;
			}
			if (i == tokens.size() && v.back() != '\'')
				throw wrong_syntax;
			this->m_values.push_back(v);
			v = "";
		} else
			this->m_values.push_back(tokens[i]);
	}
}

// LexicalAnalyzer always throws exceptions, not returning Result struct.
Operations LexicalAnalyzer::parse_expression(std::vector<std::string> tokens) {
	this->m_name = tokens[1];

	// check if table/database name is acceptable
	Operations operation = command_type(tokens[0]);

	if (operation != showtbl && operation != showdb) {
		if (this->m_name.size() == 0)     throw zero_length_name;
		if (is_keyword(this->m_name))     throw keyword;
		if (!is_legal_name(this->m_name)) throw illegal_name;
	} else if (this->m_name == "END") this->m_name = "";
	
	switch (operation) {
	case add:
		this->parse_values(tokens);
		break;
	case delrec:
		this->m_id = stoi(tokens[2]);
		break;
	case showtbl:
	case showdb:
		break;
	case use:
	case deldb:
	case deltbl:
	case crtdb:
	case crttbl: /* parse key:value to create table. */
		this->parse_columns(tokens);
		break;
	case undefined_operation: throw undefined_op;
	}
	return operation;
}
