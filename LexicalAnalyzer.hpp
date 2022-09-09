#ifndef __LEXICAL_ANALYZER_HPP__
#define __LEXICAL_ANALYZER_HPP__

#include <string>
#include <vector>
#include <set>
#include <map>
#include <exception>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <queue>

#include "Types.hpp"
#include "Error.hpp"

//#define KEYWORDS_SIZE 1

const std::set<char> ILLEGAL_SYMBOLS = {',', '(', ')', '{', '}', '!', '@', '+', '=', '#', '$', '%', '^', '&', '*', '\"', ':'};
const std::string KEYWORDS[] = {"ALL", "END", "USE", "ADD", "DELTBL", "DELDB", "SHOWTBL", "SHOWDB", "CRTTBL", "CRTDB", "TEXT", "NUMBER", "MONEY", "DATE"};
const size_t keywords_size = sizeof(KEYWORDS)/sizeof(KEYWORDS[0]);

class LexicalAnalyzer {
	std::string m_name; // name of table or database
	std::vector<std::string> m_values; // values i.e. parameters
	std::map<std::string, ColumnType> m_table_signature;
	size_t m_id;
	
	void parse_columns(std::vector<std::string> tokens);
	void parse_values(std::vector<std::string> tokens);

	bool is_keyword(const std::string &key);
	bool is_legal_name(const std::string &name);
	ColumnType column_type(std::string &st);
public:
	LexicalAnalyzer();
	Operations parse_expression(std::vector<std::string> tokens);
	const std::string &name() const;
	const std::vector<std::string> &values() const;
	
	const std::map<std::string, ColumnType> &columns() const;
	size_t id() const;
};

#endif
