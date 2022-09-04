#include "LexicalAnalyzer.hpp"
#include "Types.hpp"

LexicalAnalyzer::LexicalAnalyzer() {
}

const std::string &LexicalAnalyzer::get_name() const {
  return this->m_name;
}

const std::vector<std::string> &LexicalAnalyzer::get_values() const {
  return this->m_values;
}

const std::map<std::string, ColumnType> &LexicalAnalyzer::get_columns() const {
  //return this->m_table_signature.begin();
  return this->m_table_signature;
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
  return undefined_operation;
}


Operations LexicalAnalyzer::parse_expression(std::vector<std::string> tokens) {
  // TODO: Check if some token is KEYWORD
  if (tokens[1].size() == 0) throw std::invalid_argument("Name can't be zero"); // Check if name in tokens is zero.
  this->m_name = tokens[1];
  Operations operation;
  switch ((operation = command_type(tokens[0]))) {
    case add:
      this->m_values.clear();
      for (size_t i = 2; i < tokens.size()-1; i++) {
        auto n = ILLEGAL_SYMBOLS.find(tokens[i]);
        if (n != ILLEGAL_SYMBOLS.end()) throw std::invalid_argument("Invalid token in expression");
        this->m_values.push_back(tokens[i]);
      }
      break;
    /* all next N cases are the same: only table's or db's names matter */
    case use:
    case deldb:
    case deltbl:
    case showtbl:
    case showdb:
    case crtdb:
      //this->m_name = tokens[1];
      //break;
      return operation;
    case crttbl: /* parse key:value to create table. */
      this->m_name = tokens[1];
      this->m_table_signature.clear();
      for (size_t i = 2; i < tokens.size()-1; i++) {
        int delimeter_pos = tokens[i].find(':');
        std::string key = tokens[i].substr(0, delimeter_pos);
        std::string value = tokens[i].substr(delimeter_pos+1, tokens[i].length()-1);
        ColumnType col_type = column_type(value);
        if (col_type == undefined_type) {
          // TODO Error class for formatting and throwing errors
          std::string error = "Undefined column type: "+(value.size()==0 ? ("space after '"+key+"'") : "'"+value+"'");
          throw std::invalid_argument(error);
        }
        this->m_table_signature.insert(std::pair<std::string, ColumnType>(key, col_type));
      }
      break;
    case undefined_operation:
      std::string error = "Undefined operation: "+tokens[0];
      throw std::invalid_argument(error);
  }
  return operation;
}
