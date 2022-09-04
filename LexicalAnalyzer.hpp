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
const std::string KEYWORDS[] = {"ALL"};

class LexicalAnalyzer {
  std::string m_name; // name of table or database
  std::vector<std::string> m_values; // values i.e. parameters
  std::map<std::string, ColumnType> m_table_signature;
  
public:
  LexicalAnalyzer();
  Operations parse_expression(std::vector<std::string> tokens);
  const std::string &get_name() const;
  const std::vector<std::string> &get_values() const;
  const std::map<std::string, ColumnType> &get_columns() const;
};

#endif