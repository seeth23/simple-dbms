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

#include "Types.hpp"

//#define KEYWORDS_SIZE 1

const std::set<std::string> ILLEGAL_SYMBOLS = {",", "(", ")", "{", "}", "!", "@", "+", "=", "#", "$", "%", "^", "&", "*", "\"", "'"};
const std::string KEYWORDS[] = {"ALL", "END", "USE", "ADD", "DELTBL", "DELDB", "SHOWTBL", "SHOWDB", "CRTTBL", "CRTDB", "TEXT", "NUMBER", "MONEY", "DATE"};
const size_t keywords_size = sizeof(KEYWORDS)/sizeof(KEYWORDS[0]);


class LexicalAnalyzer {
	std::string m_name; // name of table or database
	std::vector<std::string> m_values; // values i.e. parameters
	std::map<std::string, ColumnType> m_table_signature;
	size_t m_id;
	
public:
	LexicalAnalyzer();
	Operations parse_expression(std::vector<std::string> tokens);
	const std::string &name() const;
	const std::vector<std::string> &values() const;
	const std::map<std::string, ColumnType> &columns() const;
	size_t id() const;
};

#endif
